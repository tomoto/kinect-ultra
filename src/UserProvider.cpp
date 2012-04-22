//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#include "UserProvider.h"
#include "config.h"

UserProvider::UserProvider(INuiSensor* pSensor) : AbstractKinectDataProvider(pSensor)
{
	CALL_NUI(m_pSensor->NuiSkeletonTrackingEnable(m_hNextFrameEvent, NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA));
}

UserProvider::~UserProvider()
{
}

bool UserProvider::waitForNextFrameAndLockImpl(DWORD timeout)
{
	if (SUCCEEDED(WaitForSingleObjectEx(m_hNextFrameEvent, timeout, TRUE))) {
		CALL_NUI(m_pSensor->NuiSkeletonGetNextFrame(timeout, &m_frame));
		m_pSensor->NuiTransformSmooth(&m_frame, NULL);
		m_isLocked = true;
		return true;
	} else {
		return false;
	}
}

void UserProvider::unlockImpl()
{
	ResetEvent(m_hNextFrameEvent);
	m_isLocked = false;
}

const NUI_SKELETON_DATA* UserProvider::getSkeletonData(XuUserID userID)
{
	return (userID > 0) ? &m_frame.SkeletonData[userID-1] : NULL;
}

const XuUserID UserProvider::findFirstTrackedUserID()
{
	for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
		if (m_frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
			return i+1;
		}
	}

	return 0;
}

bool UserProvider::isUserTracked(XuUserID userID)
{
	const NUI_SKELETON_DATA* skeleton = getSkeletonData(userID);
	return skeleton && skeleton->eTrackingState != NUI_SKELETON_NOT_TRACKED;
}

static float convertSkeletonTrackingStateToConfidence(NUI_SKELETON_POSITION_TRACKING_STATE state)
{
	switch (state) {
	case NUI_SKELETON_POSITION_TRACKED:
		return 1.0f;
	case NUI_SKELETON_POSITION_INFERRED:
		return 0.6f;
	default:
		return 0.0f;
	}
}

static void transformCoordinatesFromDepthToColor(const Vector4& depthCameraCoords, Vector4* pRGBCameraCoords)
{
	// Transform the coordinates based on the RGB camera instead of the depth camera
	USHORT z;
	LONG x, y;
	LONG ix, iy;
	NuiTransformSkeletonToDepthImage(depthCameraCoords, &x, &y, &z, NUI_IMAGE_RESOLUTION_640x480);
	NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, z, &ix, &iy);
	*pRGBCameraCoords = NuiTransformDepthImageToSkeleton(ix, iy, z, NUI_IMAGE_RESOLUTION_640x480);
}

const void UserProvider::getSkeletonJointInfo(XuUserID userID, XuSkeletonJointIndex jointIndex, XuSkeletonJointInfo* pJointPosition)
{
	const NUI_SKELETON_DATA* skeleton = getSkeletonData(userID);

	if (skeleton) {
		pJointPosition->fConfidence = convertSkeletonTrackingStateToConfidence(skeleton->eSkeletonPositionTrackingState[jointIndex]);
		Vector4 p;
		transformCoordinatesFromDepthToColor(skeleton->SkeletonPositions[jointIndex], &p);
		pJointPosition->position.X = p.x * 1000;
		pJointPosition->position.Y = p.y * 1000;
		pJointPosition->position.Z = p.z * 1000;
	} else {
		pJointPosition->fConfidence = 0;
	}
}
