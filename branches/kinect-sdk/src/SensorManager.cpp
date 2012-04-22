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

SensorManager::SensorManager() : m_pSensor(NULL)
{
	CALL_NUI(NuiCreateSensorByIndex(0, &m_pSensor));
	DWORD flags = NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON;
	CALL_NUI(m_pSensor->NuiInitialize(flags));
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
