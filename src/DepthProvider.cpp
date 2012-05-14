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

#ifdef XU_KINECTSDK

DepthProvider::DepthProvider(INuiSensor* pSensor) : AbstractImageStreamProvider(pSensor)
{
	CALL_SENSOR(m_pSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, 0, 2, m_hNextFrameEvent, &m_hStream));
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
		CALL_SENSOR(m_pSensor->NuiImageStreamGetNextFrame(m_hStream, timeout, &m_frame));
		CALL_SENSOR(m_frame.pFrameTexture->LockRect(0, &m_lockedRect, NULL, 0));
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
	CALL_SENSOR(m_pSensor->NuiImageStreamReleaseFrame(m_hStream, &m_frame));
	ResetEvent(m_hNextFrameEvent);
	m_isLocked = false;
}

void DepthProvider::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ)
{
	Vector4 v;
	v.x = p.X / 1000.0f;
	v.y = p.Y / 1000.0f;
	v.z = p.Z / 1000.0f;
	NuiTransformSkeletonToDepthImage(v, pX, pY, pZ, NUI_IMAGE_RESOLUTION_640x480);
}

void DepthProvider::transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint)
{
	Vector4 v = NuiTransformDepthImageToSkeleton(x, y, z, NUI_IMAGE_RESOLUTION_640x480);
	pPoint->X = v.x * 1000;
	pPoint->Y = v.y * 1000;
	pPoint->Z = v.z * 1000;

}

void DepthProvider::getFOV(float* pHFOV, float* pVFOV)
{
	*pHFOV = 1.0145f;
	*pVFOV = 0.7898f;
}

#else // XU_OPENNI

DepthProvider::DepthProvider(Context* pContext, ImageGenerator* pImageGen, UserGenerator* pUserGen) : AbstractImageStreamProvider(pContext)
{
	CALL_SENSOR( pContext->FindExistingNode(XN_NODE_TYPE_DEPTH, m_depthGen) );
	CALL_SENSOR( m_depthGen.GetAlternativeViewPointCap().SetViewPoint(*pImageGen) );

	m_pUserGen = pUserGen;

	DepthMetaData md;
	m_depthGen.GetMetaData(md);
	CHECK_ERROR(md.XRes() == 640 && md.YRes() == 480, "This camera's resolution is not supported.");
}

DepthProvider::~DepthProvider()
{
}

const XuRawUserIDPixel* DepthProvider::getUserIDData() const
{
	SceneMetaData md;
	m_pUserGen->GetUserPixels(0, md);
	return md.Data();
}

void DepthProvider::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ)
{
	// TODO performance concern

	XnPoint3D rp = { p.X, p.Y, p.Z }, pp;
	m_depthGen.ConvertRealWorldToProjective(1, &rp, &pp);
	*pX = (LONG) pp.X;
	*pY = (LONG) pp.Y;
	*pZ = (XuRawDepthPixel) pp.Z;
}

void DepthProvider::transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint)
{
	// TODO performance concern
	
	XnPoint3D pp = { XnFloat(x), XnFloat(y), XnFloat(z) }, rp;
	m_depthGen.ConvertProjectiveToRealWorld(1, &pp, &rp);
	pPoint->X = rp.X;
	pPoint->Y = rp.Y;
	pPoint->Z = rp.Z;
}

void DepthProvider::getFOV(float* pHFOV, float* pVFOV)
{
	XnFieldOfView fov;
	m_depthGen.GetFieldOfView(fov);
	*pHFOV = (float) fov.fHFOV;
	*pVFOV = (float) fov.fVFOV;
}

#endif
