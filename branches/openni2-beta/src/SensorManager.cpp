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

#include "SensorManager.h"
#include "util.h"

#ifdef XU_KINECTSDK

#include "nui_error.h"

static void CALLBACK dummyDeviceStatusCallback(HRESULT, const OLECHAR*, const OLECHAR*, void*)
{
}

static INuiSensor* findFirstAvailableSensor()
{
	// Register dummy device status callback to avoid freezing. Kinect SDK's bug?
	NuiSetDeviceStatusCallback(dummyDeviceStatusCallback, NULL);

	int sensorCount;
	CALL_SENSOR(NuiGetSensorCount(&sensorCount));
	CHECK_ERROR(sensorCount > 0, "No devices are connected.");

	INuiSensor* pSensor = NULL;
	for (int i = 0; i < sensorCount; i++) {
		CALL_SENSOR(NuiCreateSensorByIndex(i, &pSensor));
		DWORD flags = NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON;
		HRESULT hr = pSensor->NuiInitialize(flags);
		if (FAILED(hr)) {
			char msg[1024];
			getNuiErrorString(hr, msg, sizeof(msg)) || strcpy(msg, "?");
			printf("Device #%d is not available: %s [%08x]\n", i, msg, hr);
			pSensor->Release();
			pSensor = NULL;
			CHECK_ERROR(i < sensorCount-1, "No available devices."); // this was the last one
			continue; // retry
		}
		break; // got one
	}

	return pSensor;
}

SensorManager::SensorManager() : m_pSensor(NULL)
{
	m_pSensor = findFirstAvailableSensor();

	m_imageProvider = new ImageProvider(m_pSensor);
	m_depthProvider = new DepthProvider(m_pSensor);
	m_userProvider = new UserProvider(m_pSensor);
}

SensorManager::~SensorManager()
{
	if (m_pSensor) {
		m_pSensor->NuiShutdown();
		m_pSensor->Release();
		m_pSensor = NULL;
	}
}

bool SensorManager::waitAllForNextFrameAndLock()
{
	return m_imageProvider->waitForNextFrameAndLock() &&
		m_depthProvider->waitForNextFrameAndLock() &&
		m_userProvider->waitForNextFrameAndLock();
}

void SensorManager::unlock()
{
	m_userProvider->unlock();
	m_depthProvider->unlock();
	m_imageProvider->unlock();
}

#else // XU_OPENNI

SensorManager::SensorManager()
{
	ScriptNode scriptNode;
	CALL_SENSOR( m_context.InitFromXmlFile(getResourceFile("config", "OpenNIConfig.xml").c_str(), scriptNode) );
	CALL_SENSOR( m_context.SetGlobalMirror(TRUE) );
	m_imageProvider = new ImageProvider(&m_context);
	m_userProvider = new UserProvider(&m_context);
	m_depthProvider = new DepthProvider(&m_context, m_imageProvider->getGenerator(), m_userProvider->getGenerator());

	m_context.StartGeneratingAll(); // too early? let's see if it works.
}

SensorManager::~SensorManager()
{
}

bool SensorManager::waitAllForNextFrameAndLock()
{
	return m_context.WaitAndUpdateAll() == XN_STATUS_OK;
}

void SensorManager::unlock()
{
	// nothing to do
}

#endif
