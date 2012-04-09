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

#include "DepthProvider.h"
#include "config.h"

DepthProvider::DepthProvider(INuiSensor* pSensor) : AbstractImageStreamProvider(pSensor)
{
	CALL_NUI(m_pSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, 0, 2, m_hNextFrameEvent, &m_hStream));
	m_data = new XuRawDepthPixel[X_RES * Y_RES];
	//m_depthData = new XuDepthPixel[X_RES * Y_RES];
	//m_userIdData = new XuUserID[X_RES * Y_RES];

	assert(X_RES == 640 && Y_RES == 480);
}

DepthProvider::~DepthProvider()
{
}

bool DepthProvider::waitForNextFrameAndLockImpl(DWORD timeout)
{
	if (SUCCEEDED(WaitForSingleObjectEx(m_hNextFrameEvent, timeout, TRUE))) {
		CALL_NUI(m_pSensor->NuiImageStreamGetNextFrame(m_hStream, timeout, &m_frame));
		CALL_NUI(m_frame.pFrameTexture->LockRect(0, &m_lockedRect, NULL, 0));
		XuRawDepthPixel* data = (XuRawDepthPixel*) m_lockedRect.pBits;

		memset(m_data, 0, X_RES * Y_RES * sizeof(XuRawDepthPixel));

		for (LONG y = 0; y < 240; y++) {
			XuRawDepthPixel* sp = data + y * 320;
			for (LONG x = 0; x < 320; x++) {
				LONG ix, iy;
				XuRawDepthPixel d = *sp++;
				NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_320x240, NULL, x, y, d, &ix, &iy);
				if (ix >= 0 && ix <= 640-2 && iy >= 0 && iy <= 480-2) {
					int p = ix + iy * 640;
					m_data[p] = m_data[p+1] = m_data[p+640] = m_data[p+641] = d;
					//m_depthData[p] = m_depthData[p+1] = m_depthData[p+640] = m_depthData[p+641] = NuiDepthPixelToDepth(d);
					//m_userIdData[p] = m_userIdData[p+1] = m_userIdData[p+640] = m_userIdData[p+641] = NuiDepthPixelToPlayerIndex(d);
				}
			}
		}

		m_isLocked = true;
		return true;
	} else {
		return false;
	}
}

void DepthProvider::unlockImpl()
{
	CALL_NUI(m_pSensor->NuiImageStreamReleaseFrame(m_hStream, &m_frame));
	ResetEvent(m_hNextFrameEvent);
	m_isLocked = false;
}

void DepthProvider::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, USHORT* pZ)
{
	Vector4 v;
	v.x = p.X / 1000.0f;
	v.y = p.Y / 1000.0f;
	v.z = p.Z / 1000.0f;
	NuiTransformSkeletonToDepthImage(v, pX, pY, pZ, NUI_IMAGE_RESOLUTION_640x480);
}

void DepthProvider::transformDepthImageToSkeleton(LONG x, LONG y, USHORT z, XV3* pPoint)
{
	Vector4 v = NuiTransformDepthImageToSkeleton(x, y, z, NUI_IMAGE_RESOLUTION_640x480);
	pPoint->X = v.x * 1000;
	pPoint->Y = v.y * 1000;
	pPoint->Z = v.z * 1000;

}
