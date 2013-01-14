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

#ifndef _IMAGE_PROVIDER_H_
#define _IMAGE_PROVIDER_H_

#include "common.h"
#include "AbstractImageStreamProvider.h"

class ImageProvider {
public:
	ImageProvider() {}
	virtual ~ImageProvider() {}

	virtual const XuRawColorPixel* getData() const = 0;
};

#ifdef XU_KINECTSDK

class ImageProviderImpl : public ImageProvider, public AbstractImageStreamProvider {
public:
	ImageProviderImpl(INuiSensor* pSensor);
	~ImageProviderImpl();

	const XuRawColorPixel* getData() const { return (const XuRawColorPixel*) m_lockedRect.pBits; }

protected:
	virtual bool waitForNextFrameAndLockImpl(DWORD timeout);
	virtual void unlockImpl();
};

#elif defined XU_OPENNI2

class ImageProviderImpl : public ImageProvider, public AbstractImageStreamProvider {
public:
	ImageProviderImpl(openni::Device* pDevice);
	~ImageProviderImpl();

	bool waitForNextFrame();
	const XuRawColorPixel* getData() const { return (const XuRawColorPixel*) m_frameRef.getData(); }
};

#else // XU_OPENNI

class ImageProviderImpl : public ImageProvider, public AbstractImageStreamProvider {
private:
	ImageGenerator m_imageGen;
public:
	ImageProviderImpl(Context* context);
	~ImageProviderImpl();

	ImageGenerator* getGenerator() { return &m_imageGen; }
	const XuRawColorPixel* getData() const { return (const XuRawColorPixel*) m_imageGen.GetRGB24ImageMap(); }
};

#endif
#endif
