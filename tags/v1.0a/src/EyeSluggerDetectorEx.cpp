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

#include "EyeSluggerDetectorEx.h"
#include "util.h"

EyeSluggerDetectorEx::EyeSluggerDetectorEx(HenshinDetector* henshinDetector, EyeSluggerRendererEx* renderer)
	: EyeSluggerDetector(henshinDetector, renderer)
{
	m_renderer = renderer;
	m_stage = STAGE_NORMAL;
}

EyeSluggerDetectorEx::~EyeSluggerDetectorEx()
{
}

void EyeSluggerDetectorEx::onDetectPre(float dt)
{
	if (m_henshinDetector->getStage() != HenshinDetector::STAGE_HENSHINED) {
		return;
	}

	if (m_renderer->isShot()) {
		return; // already shot, nothing to do
	}

	switch (m_stage) { // UK
		case STAGE_NORMAL:
			EyeSluggerDetector::onDetectPre(dt);
			break;
		case STAGE_HOLDING_IN_HAND:
		case STAGE_HOLDING_IN_HAND_POST:
			processPosesInHand(dt);
			break;
		case STAGE_HOLDING_IN_AIR:
			processPosesInAir(dt);
			break;
	}
}

void EyeSluggerDetectorEx::shootSlugger(float velocity, float rotation, int traceDencity)
{
	float scale = (m_stage == STAGE_HOLDING_IN_AIR) ? -1.0f : 1;
	EyeSluggerDetector::shootSlugger(velocity * scale, rotation, traceDencity);
}

void EyeSluggerDetectorEx::processUnposing(float dt)
{
	if (isLeftArmStraightToFront() && m_energy > 0) {
		holdSlugger();
		m_stage = STAGE_HOLDING_IN_HAND;
		const float DEENERGIZE_SPEED = 1;
		deenegize(dt, DEENERGIZE_SPEED);
	} else {
		EyeSluggerDetector::processUnposing(dt);
	}
}

bool EyeSluggerDetectorEx::isLeftArmStraightToFront()
{
	UserDetector* ud = m_henshinDetector->getUserDetector();
	XuSkeletonJointInfo jls, jle, jlh;
	ud->getSkeletonJointInfo(XU_SKEL_LEFT_SHOULDER, &jls);
	ud->getSkeletonJointInfo(XU_SKEL_LEFT_ELBOW, &jle);
	ud->getSkeletonJointInfo(XU_SKEL_LEFT_HAND, &jlh);

	if (!isConfident(jls) || !isConfident(jle) || !isConfident(jlh)) {
		return false;
	}

	XV3 pls(jls.position), ple(jle.position), plh(jlh.position);
	XV3 vlse(ple - pls), vleh(plh - ple);

	const float ARM_ANGLE_THRESHOLD = 0.85f;

	bool isLeftUpperArmHorizontal =
		m_henshinDetector->getUserDetector()->getForwardVector().dotNormalized(vlse) > ARM_ANGLE_THRESHOLD;
	bool isLeftArmStraight = vlse.dotNormalized(vleh) > ARM_ANGLE_THRESHOLD;

	return isLeftUpperArmHorizontal && isLeftArmStraight;
}

void EyeSluggerDetectorEx::holdSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRendererEx::HOLD_IN_HAND);
	m_renderer->setEnergy(0);
	// setEnergy(0); // unnecessary, just wait for natural cool down
}

void EyeSluggerDetectorEx::restoreSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRendererEx::HOLD_NORMAL);
	setEnergy(0);
}

void EyeSluggerDetectorEx::fixSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRendererEx::HOLD_IN_AIR);
}

void EyeSluggerDetectorEx::processPosingInHand(float dt)
{
	if (m_stage == STAGE_HOLDING_IN_HAND_POST) {
		return;
	}

	const float ENEGIZE_SPEED = 0.5f;
	enegize(dt, ENEGIZE_SPEED);

	if (m_energy >= 1) {
		fixSlugger();
		m_stage = STAGE_HOLDING_IN_HAND_POST;
	}
}

void EyeSluggerDetectorEx::processUnposingInHand(float dt)
{
	if (m_stage == STAGE_HOLDING_IN_HAND_POST) {
		m_stage = STAGE_HOLDING_IN_AIR;
		return;
	}

	const float DEENEGIZE_SPEED = 1;
	deenegize(dt, DEENEGIZE_SPEED);
	
	if (isHandBackOfHead()) {
		restoreSlugger();
		m_stage = STAGE_NORMAL;
	}
}

bool EyeSluggerDetectorEx::processPosesInHand(float dt)
{
	XV3 pr = m_userDetector->getSkeletonJointPosition(XU_SKEL_RIGHT_HAND);
	XV3 pl = m_userDetector->getSkeletonJointPosition(XU_SKEL_LEFT_HAND);
	const float HAND_DISTANCE_THRESHOLD = 150;
	bool areHandsClose = pr.distance2(pl) < square(HAND_DISTANCE_THRESHOLD);

	if (areHandsClose) {
		processPosingInHand(dt);
		return true;
	} else {
		processUnposingInHand(dt);
		return false;
	}
}

void EyeSluggerDetectorEx::processPosingInAir(float dt)
{
	const float VELOCITY = 5000;
	const float ROTATION = 5.2f;
	const int DENCITY = 3;
	shootSlugger(VELOCITY, ROTATION, DENCITY);
	m_stage = STAGE_NORMAL;
}

void EyeSluggerDetectorEx::processUnposingInAir(float dt)
{
	// TODO release ES at some time
}

bool EyeSluggerDetectorEx::processPosesInAir(float dt)
{
	XV3 p = m_renderer->getOrigin();
	XV3 pr = m_userDetector->getSkeletonJointPosition(XU_SKEL_RIGHT_HAND);
	XV3 pl = m_userDetector->getSkeletonJointPosition(XU_SKEL_LEFT_HAND);
	const float HAND_DISTANCE_THRESHOLD = 200;
	bool isEitherHandClose = std::min(p.distance2(pr), p.distance2(pl)) < square(HAND_DISTANCE_THRESHOLD);

	if (isEitherHandClose) {
		processPosingInAir(dt);
		return true;
	} else {
		processUnposingInAir(dt);
		return false;
	}
}
