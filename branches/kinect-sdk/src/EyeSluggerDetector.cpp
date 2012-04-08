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

	m_stage = STAGE_NORMAL;
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

bool EyeSluggerDetector::isLeftArmStraightToFront()
{
	UserDetector* ud = m_henshinDetector->getUserDetector();
	XnSkeletonJointPosition jls, jle, jlh;
	ud->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER, &jls);
	ud->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW, &jle);
	ud->getSkeletonJointPosition(XN_SKEL_LEFT_HAND, &jlh);

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

void EyeSluggerDetector::shootSlugger(float velocity, float rotation, int traceDencity)
{
	m_renderer->setHoldMode(EyeSluggerRenderer::HOLD_ON_HEAD);

	float scale = (m_stage == STAGE_HOLDING_IN_AIR) ? -1.0f : 1;

	XV3 v = m_renderer->getForwardVector() * scale;
	m_renderer->shoot(v * velocity, rotation, traceDencity);
	setEnergy(0, true);
}

void EyeSluggerDetector::holdSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRenderer::HOLD_IN_HAND);
	m_renderer->setEnergy(0);
	// setEnergy(0); // unnecessary, just wait for natural cool down
}

void EyeSluggerDetector::restoreSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRenderer::HOLD_ON_HEAD);
	setEnergy(0);
}

void EyeSluggerDetector::posingNormal(float dt)
{
	const float ENERGIZE_SPEED = 2;
	enegize(dt, ENERGIZE_SPEED);
}

void EyeSluggerDetector::unposingNormal(float dt)
{
	if (m_energy >= 1) {
		if (!isLeftArmStraightToFront()) {
			const float VELOCITY = 4000;
			const float ROTATION = 2.2f;
			const int DENCITY = 1;
			shootSlugger(VELOCITY, ROTATION, DENCITY);
			m_stage = STAGE_NORMAL;
			return;
		}
	}

	if (m_energy > 0) {
		if (isLeftArmStraightToFront()) {
			holdSlugger();
			m_stage = STAGE_HOLDING_IN_HAND;
		}
	}

	const float DEENERGIZE_SPEED = 1;
	deenegize(dt, DEENERGIZE_SPEED);
}

bool EyeSluggerDetector::isArmTopOfHead()
{
	// TODO remove copy & paste
	XnSkeletonJointPosition hands[2], head;
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND, &hands[0]);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND, &hands[1]);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_HEAD, &head);

	bool handsPosed[2];
	const float ES_HEAD_TOP_OFFSET = 100;
	for (int i = 0; i < 2; i++) {
		handsPosed[i] = isConfident(hands[i]) && (hands[i].position.Y > head.position.Y + ES_HEAD_TOP_OFFSET);
	}
	
	return handsPosed[0] || handsPosed[1];
}

bool EyeSluggerDetector::isHandBackOfHead()
{
	XnSkeletonJointPosition hands[2], head;
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND, &hands[0]);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND, &hands[1]);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_HEAD, &head);

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

bool EyeSluggerDetector::processPosingNomal(float dt)
{
	if (isHandBackOfHead()) {
		posingNormal(dt);
		return true;
	} else {
		unposingNormal(dt);
		return false;
	}
}

void EyeSluggerDetector::fixSlugger()
{
	m_renderer->setHoldMode(EyeSluggerRenderer::HOLD_IN_AIR);
}

void EyeSluggerDetector::posingInHand(float dt)
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

void EyeSluggerDetector::unposingInHand(float dt)
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

bool EyeSluggerDetector::processPosingInHand(float dt)
{
	XV3 pr = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
	XV3 pl = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND);
	const float HAND_DISTANCE_THRESHOLD = 150;
	bool areHandsClose = pr.distance2(pl) < square(HAND_DISTANCE_THRESHOLD);

	if (areHandsClose) {
		posingInHand(dt);
		return true;
	} else {
		unposingInHand(dt);
		return false;
	}
}

void EyeSluggerDetector::posingInAir(float dt) {
	const float VELOCITY = 5000;
	const float ROTATION = 5.2f;
	const int DENCITY = 3;
	shootSlugger(VELOCITY, ROTATION, DENCITY);
	m_stage = STAGE_NORMAL;
}

void EyeSluggerDetector::unposingInAir(float dt)
{
	// TODO release ES at some time
}

bool EyeSluggerDetector::processPosingInAir(float dt)
{
	XV3 p = m_renderer->getOrigin();
	XV3 pr = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
	XV3 pl = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND);
	const float HAND_DISTANCE_THRESHOLD = 200;
	bool isEitherHandClose = std::min(p.distance2(pr), p.distance2(pl)) < square(HAND_DISTANCE_THRESHOLD);

	if (isEitherHandClose) {
		posingInAir(dt);
		return true;
	} else {
		unposingInAir(dt);
		return false;
	}
}

void EyeSluggerDetector::onDetectPre(float dt)
{
	if (m_henshinDetector->getStage() != HenshinDetector::STAGE_HENSHINED) {
		return;
	}

	if (m_renderer->isShot()) {
		return; // already shot, nothing to do
	}

	switch (m_stage) {
		case STAGE_NORMAL:
			processPosingNomal(dt);
			break;
		case STAGE_HOLDING_IN_HAND:
		case STAGE_HOLDING_IN_HAND_POST:
			processPosingInHand(dt);
			break;
		case STAGE_HOLDING_IN_AIR:
			processPosingInAir(dt);
			break;
	}
}
