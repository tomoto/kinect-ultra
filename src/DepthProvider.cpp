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

DepthProviderImpl::DepthProviderImpl(INuiSensor* pSensor) : AbstractImageStreamProvider(pSensor)
{
	CALL_SENSOR(m_pSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, 0, 2, m_hNextFrameEvent, &m_hStream));
	m_data = new XuRawDepthPixel[X_RES * Y_RES];
	//m_depthData = new XuDepthPixel[X_RES * Y_RES];
	//m_userIdData = new XuUserID[X_RES * Y_RES];

	assert(X_RES == 640 && Y_RES == 480);
}

DepthProviderImpl::~DepthProviderImpl()
{
}

bool DepthProviderImpl::waitForNextFrameAndLockImpl(DWORD timeout)
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

void DepthProviderImpl::unlockImpl()
{
	CALL_SENSOR(m_pSensor->NuiImageStreamReleaseFrame(m_hStream, &m_frame));
	ResetEvent(m_hNextFrameEvent);
	m_isLocked = false;
}

void DepthProviderImpl::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ)
{
	Vector4 v;
	v.x = p.X / 1000.0f;
	v.y = p.Y / 1000.0f;
	v.z = p.Z / 1000.0f;
	NuiTransformSkeletonToDepthImage(v, pX, pY, pZ, NUI_IMAGE_RESOLUTION_640x480);
}

void DepthProviderImpl::transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint)
{
	Vector4 v = NuiTransformDepthImageToSkeleton(x, y, z, NUI_IMAGE_RESOLUTION_640x480);
	pPoint->X = v.x * 1000;
	pPoint->Y = v.y * 1000;
	pPoint->Z = v.z * 1000;

}

void DepthProviderImpl::getFOV(float* pHFOV, float* pVFOV)
{
	*pHFOV = 1.0145f;
	*pVFOV = 0.7898f;
}

#elif XU_OPENNI2

DepthProviderImpl::DepthProviderImpl(openni::Device* pDevice) : AbstractImageStreamProvider(pDevice)
{
	CALL_SENSOR( m_stream.create(*pDevice, openni::SensorType::SENSOR_DEPTH ) );

	openni::VideoMode vm;
	vm.setFps(30);
	vm.setResolution(640, 480);
	vm.setPixelFormat(openni::PixelFormat::PIXEL_FORMAT_DEPTH_1_MM);
	CALL_SENSOR( m_stream.setVideoMode(vm) );

	CALL_SENSOR( m_stream.start() );

	CALL_SENSOR( m_userTracker.create(pDevice) );
}

DepthProviderImpl::~DepthProviderImpl()
{
	m_userTracker.destroy();
	m_stream.destroy();
}

void DepthProviderImpl::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ)
{
	XV3 pp;
	m_userTracker.convertJointCoordinatesToDepth(p.X, p.Y, p.Z, &pp.X, &pp.Y);
	*pX = (LONG) pp.X;
	*pY = (LONG) pp.Y;
	*pZ = (XuRawDepthPixel) p.Z;
}

void DepthProviderImpl::transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint)
{
	m_userTracker.convertDepthCoordinatesToJoint(int(x), int(y), int(z), &(pPoint->X), &(pPoint->Y));
	pPoint->Z = z;
}

void DepthProviderImpl::getFOV(float* pHFOV, float* pVFOV)
{
	*pHFOV = m_stream.getHorizontalFieldOfView();
	*pVFOV = m_stream.getVerticalFieldOfView();
}

bool DepthProviderImpl::waitForNextFrame() {
	CALL_SENSOR( m_stream.readFrame(&m_frameRef) );
	CALL_SENSOR( m_userTracker.readFrame(&m_userFrameRef) );
	return true;
}

#else // XU_OPENNI

DepthProviderImpl::DepthProviderImpl(Context* pContext, ImageGenerator* pImageGen, UserGenerator* pUserGen) : AbstractImageStreamProvider(pContext)
{
	CALL_SENSOR( pContext->FindExistingNode(XN_NODE_TYPE_DEPTH, m_depthGen) );
	CALL_SENSOR( m_depthGen.GetAlternativeViewPointCap().SetViewPoint(*pImageGen) );

	m_pUserGen = pUserGen;

	DepthMetaData md;
	m_depthGen.GetMetaData(md);
	CHECK_ERROR(md.XRes() == 640 && md.YRes() == 480, "This camera's resolution is not supported.");
}

DepthProviderImpl::~DepthProviderImpl()
{
}

const XuRawUserIDPixel* DepthProviderImpl::getUserIDData() const
{
	SceneMetaData md;
	m_pUserGen->GetUserPixels(0, md);
	return md.Data();
}

void DepthProviderImpl::transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ)
{
	// TODO performance concern

	XnPoint3D rp = { p.X, p.Y, p.Z }, pp;
	m_depthGen.ConvertRealWorldToProjective(1, &rp, &pp);
	*pX = (LONG) pp.X;
	*pY = (LONG) pp.Y;
	*pZ = (XuRawDepthPixel) pp.Z;
}

void DepthProviderImpl::transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint)
{
	// TODO performance concern
	
	XnPoint3D pp = { XnFloat(x), XnFloat(y), XnFloat(z) }, rp;
	m_depthGen.ConvertProjectiveToRealWorld(1, &pp, &rp);
	pPoint->X = rp.X;
	pPoint->Y = rp.Y;
	pPoint->Z = rp.Z;
}

void DepthProviderImpl::getFOV(float* pHFOV, float* pVFOV)
{
	XnFieldOfView fov;
	m_depthGen.GetFieldOfView(fov);
	*pHFOV = (float) fov.fHFOV;
	*pVFOV = (float) fov.fVFOV;
}

#endif
