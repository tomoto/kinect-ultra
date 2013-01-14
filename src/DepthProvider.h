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

#ifndef _DEPTH_PROVIDER_H_
#define _DEPTH_PROVIDER_H_

#include "common.h"
#include "AbstractImageStreamProvider.h"
#include "vec.h"

class DepthProvider
{
public:
	DepthProvider() {}
	virtual ~DepthProvider() {}

	virtual const XuRawDepthPixel* getData() const = 0;
	virtual const XuRawUserIDPixel* getUserIDData() const = 0;

	virtual void transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ) = 0;
	virtual void transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint) = 0;

	virtual void getFOV(float* pHFOV, float* pVFOV) = 0;
};

#ifdef XU_KINECTSDK

class DepthProviderImpl : public DepthProvider, public AbstractImageStreamProvider
{
private:
	XuRawDepthPixel* m_data;

public:
	DepthProviderImpl(INuiSensor* pSensor);
	~DepthProviderImpl();

	const XuRawDepthPixel* getData() const { return m_data; }
	const XuRawUserIDPixel* getUserIDData() const { return m_data; }

	void transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ);
	void transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint);

	void getFOV(float* pHFOV, float* pVFOV);

protected:
	virtual bool waitForNextFrameAndLockImpl(DWORD timeout);
	virtual void unlockImpl();
};

#elif XU_OPENNI2

class DepthProviderImpl : public DepthProvider, public AbstractImageStreamProvider
{
private:
	typedef AbstractImageStreamProvider SuperClass;

	nite::UserTracker m_userTracker;
	nite::UserTrackerFrameRef m_userFrameRef;

public:
	DepthProviderImpl(openni::Device* pDevice);
	~DepthProviderImpl();

	nite::UserTracker* getUserTracker() { return &m_userTracker; }
	nite::UserTrackerFrameRef* getUserTrackerFrame() { return &m_userFrameRef; }

	const XuRawDepthPixel* getData() const { return (XuRawDepthPixel*) m_frameRef.getData(); }
	const XuRawUserIDPixel* getUserIDData() const { return m_userFrameRef.getUserMap().getPixels(); }

	void transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ);
	void transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint);

	void getFOV(float* pHFOV, float* pVFOV);

	bool waitForNextFrame();
};

#else // XU_OPENNI

class DepthProviderImpl : public DepthProvider, public AbstractImageStreamProvider
{
private:
	DepthGenerator m_depthGen;
	UserGenerator* m_pUserGen;

public:
	DepthProviderImpl(Context* pContext, ImageGenerator* pImageGen, UserGenerator* pUserGen);
	~DepthProviderImpl();

	const XuRawDepthPixel* getData() const { return m_depthGen.GetDepthMap(); }
	const XuRawUserIDPixel* getUserIDData() const;

	void transformSkeletonToDepthImage(const XV3& p, LONG* pX, LONG* pY, XuRawDepthPixel* pZ);
	void transformDepthImageToSkeleton(LONG x, LONG y, XuRawDepthPixel z, XV3* pPoint);

	void getFOV(float* pHFOV, float* pVFOV);
};

#endif

#endif
