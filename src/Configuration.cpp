//@COPYRIGHT@//
//
// Copyright (c) 2011, Tomoto S. Washio
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


#include "Configuration.h"

Configuration* Configuration::getInstance()
{
	static Configuration s_instance;
	return &s_instance;
}

Configuration::Configuration()
{
	m_triggerHappyMode = THMODE_NORMAL;
}

Configuration::~Configuration()
{
}

Configuration::TriggerHappyMode Configuration::getTriggerHappyMode() const
{
	return m_triggerHappyMode;
}

const char* s_getTriggerHappyModeDisplayName(Configuration::TriggerHappyMode value)
{
	static const char* s_displayNames[] = { "Normal", "Happy", "Happier", "-" };
	return s_displayNames[value];
}

void Configuration::setTriggerHappyMode(TriggerHappyMode value)
{
	m_triggerHappyMode = value;
	printf("Trigger Happy Mode = %s\n", s_getTriggerHappyModeDisplayName(value));
}

void Configuration::changeTriggerHappyMode()
{
	setTriggerHappyMode(TriggerHappyMode((m_triggerHappyMode + 1) % THMODE_MAX));
}

float Configuration::getTriggerHappyLevel() const
{
	return m_triggerHappyMode / 2.0f;
}
