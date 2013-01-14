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

#include "ImageProvider.h"

#ifdef XU_KINECTSDK

ImageProvider::ImageProvider(INuiSensor* pSensor) : AbstractImageStreamProvider(pSensor)
{
	CALL_SENSOR(m_pSensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, m_hNextFrameEvent, &m_hStream));
}

ImageProvider::~ImageProvider()
{
}

bool ImageProvider::waitForNextFrameAndLockImpl(DWORD timeout)
{
	if (SUCCEEDED(WaitForSingleObjectEx(m_hNextFrameEvent, timeout, TRUE))) {
		CALL_SENSOR(m_pSensor->NuiImageStreamGetNextFrame(m_hStream, timeout, &m_frame));
		CALL_SENSOR(m_frame.pFrameTexture->LockRect(0, &m_lockedRect, NULL, 0));
		m_isLocked = true;
		return true;
	} else {
		return false;
	}
}

void ImageProvider::unlockImpl()
{
	CALL_SENSOR(m_pSensor->NuiImageStreamReleaseFrame(m_hStream, &m_frame));
	ResetEvent(m_hNextFrameEvent);
	m_isLocked = false;
}

#else // XU_OPENNI

ImageProvider::ImageProvider(Context* pContext) : AbstractImageStreamProvider(pContext)
{
	CALL_SENSOR( pContext->FindExistingNode(XN_NODE_TYPE_IMAGE, m_imageGen) );

	ImageMetaData md;
	m_imageGen.GetMetaData(md);
	CHECK_ERROR(md.PixelFormat() == XN_PIXEL_FORMAT_RGB24, "This camera's data format is not supported.");
	CHECK_ERROR(md.XRes() == 640 && md.YRes() == 480, "This camera's resolution is not supported.");
}

ImageProvider::~ImageProvider()
{
}

#endif
