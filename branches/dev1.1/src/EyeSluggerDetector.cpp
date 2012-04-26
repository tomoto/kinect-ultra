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

#include "EyeSluggerDetector.h"
#include "util.h"

EyeSluggerDetector::EyeSluggerDetector(HenshinDetector* henshinDetector, EyeSluggerRenderer* renderer)
	: AbstractPoseDetector(henshinDetector->getUserDetector())
{
	m_henshinDetector = henshinDetector;
	m_renderer = renderer;

	m_energy = 0;
	m_renderer->setEnergy(0);
}

EyeSluggerDetector::~EyeSluggerDetector()
{
}

void EyeSluggerDetector::enegize(float dt, float speed)
{
	m_energy = std::min(m_energy + speed * dt, 1.0f);
	m_renderer->setEnergy(m_energy);
}

void EyeSluggerDetector::deenegize(float dt, float speed)
{
	m_energy = std::max(m_energy - speed * dt, 0.0f);
	m_renderer->setEnergy(m_energy);
}

void EyeSluggerDetector::setEnergy(float e, bool delay)
{
	m_energy = e;

	if (!delay) {
		m_renderer->setEnergy(m_energy);
	}
}

void EyeSluggerDetector::shootSlugger(float velocity, float rotation, int traceDencity)
{
	XV3 v = m_renderer->getForwardVector();
	m_renderer->shoot(v * velocity, rotation, traceDencity);
	setEnergy(0, true);
}

void EyeSluggerDetector::processPosing(float dt)
{
	const float ENERGIZE_SPEED = 2;
	enegize(dt, ENERGIZE_SPEED);
}

void EyeSluggerDetector::processUnposing(float dt)
{
	if (m_energy >= 1) {
		const float VELOCITY = 4000;
		const float ROTATION = 2.2f;
		const int DENCITY = 1;
		shootSlugger(VELOCITY, ROTATION, DENCITY);
	} else {
		const float DEENERGIZE_SPEED = 1;
		deenegize(dt, DEENERGIZE_SPEED);
	}
}

bool EyeSluggerDetector::isArmTopOfHead()
{
	XuSkeletonJointInfo hands[2], head;
	m_userDetector->getSkeletonJointInfo(XU_SKEL_RIGHT_HAND, &hands[0]);
	m_userDetector->getSkeletonJointInfo(XU_SKEL_LEFT_HAND, &hands[1]);
	m_userDetector->getSkeletonJointInfo(XU_SKEL_HEAD, &head);

	bool handsPosed[2];
	const float ES_HEAD_TOP_OFFSET = 100;
	for (int i = 0; i < 2; i++) {
		handsPosed[i] = isConfident(hands[i]) && (hands[i].position.Y > head.position.Y + ES_HEAD_TOP_OFFSET);
	}
	
	return handsPosed[0] || handsPosed[1];
}

bool EyeSluggerDetector::isHandBackOfHead()
{
	XuSkeletonJointInfo hands[2], head;
	m_userDetector->getSkeletonJointInfo(XU_SKEL_RIGHT_HAND, &hands[0]);
	m_userDetector->getSkeletonJointInfo(XU_SKEL_LEFT_HAND, &hands[1]);
	m_userDetector->getSkeletonJointInfo(XU_SKEL_HEAD, &head);

	XV3 fv = m_userDetector->getForwardVector();
	const float ES_GRAB_THRESHOLD = 200;
	bool handsPosed[2];
	for (int i = 0; i < 2; i++) {
		if (isConfident(hands[i])) {
			XV3 headToHand(XV3(hands[i].position) - XV3(head.position));
			handsPosed[i] = headToHand.magnitude() < ES_GRAB_THRESHOLD && fv.dot(headToHand) < 0;
		} else {
			handsPosed[i] = false;
		}
	}

	return handsPosed[0] || handsPosed[1];
}

bool EyeSluggerDetector::processPoses(float dt)
{
	if (isHandBackOfHead()) {
		processPosing(dt);
		return true;
	} else {
		processUnposing(dt);
		return false;
	}
}

void EyeSluggerDetector::onDetectPre(float dt)
{
	if (m_renderer->isShot()) {
		return; // already shot, nothing to do
	}

	processPoses(dt);
}
