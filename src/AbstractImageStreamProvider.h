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

#ifndef _ABSTRACT_IMAGE_STREAM_PROVIDER_H_
#define _ABSTRACT_IMAGE_STREAM_PROVIDER_H_

#include "common.h"
#include "AbstractSensorDataProvider.h"

#ifdef XU_KINECTSDK

class AbstractImageStreamProvider : public AbstractSensorDataProvider
{
protected:
	HANDLE m_hStream;
	NUI_IMAGE_FRAME m_frame;
	NUI_LOCKED_RECT m_lockedRect;

public:
	AbstractImageStreamProvider(INuiSensor* pSensor) : AbstractSensorDataProvider(pSensor), m_hStream(NULL)
	{
	}

	virtual ~AbstractImageStreamProvider() {
	}
};

#elif defined XU_OPENNI2

class AbstractImageStreamProvider : public AbstractSensorDataProvider
{
protected:
	openni::VideoStream m_stream;
	openni::VideoFrameRef m_frameRef;

public:
	AbstractImageStreamProvider(openni::Device* pDevice) : AbstractSensorDataProvider(pDevice)
	{
	}

	virtual ~AbstractImageStreamProvider() {
	}
};

#else // XU_OPENNI

class AbstractImageStreamProvider : public AbstractSensorDataProvider
{
public:
	AbstractImageStreamProvider(Context* pContext) : AbstractSensorDataProvider(pContext)
	{
	}

	virtual ~AbstractImageStreamProvider() {
	}
};

#endif

#endif
