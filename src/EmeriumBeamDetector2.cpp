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

#include "EmeriumBeamDetector2.h"
#include "util.h"

EmeriumBeamDetector2::EmeriumBeamDetector2(DepthProvider* depthProvider, HenshinDetector* henshinDetector, AbstractSimpleBeamRenderer* beamRenderer)
: AbstractEmeriumBeamDetector(depthProvider, henshinDetector, beamRenderer)
{
}

EmeriumBeamDetector2::~EmeriumBeamDetector2()
{
}

float EmeriumBeamDetector2::getNeckDistanceThreshold()
{
	return 200.0f + 100.0f * getConfiguration()->getTriggerHappyLevel();
}

float EmeriumBeamDetector2::getArmAngleThreshold()
{
	return 0.9f - 0.1f * getConfiguration()->getTriggerHappyLevel();
}

bool EmeriumBeamDetector2::isPosing(float dt)
{
	XV3 pl0 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW);
	XV3 pl1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND);
	XV3 plsh = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER);
	XV3 prsh = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER);
	XV3 pr1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
	XV3 vl = pl1 - pl0; // left arm
	XV3 vsh = prsh - plsh; // shoulder line
	XV3 pn = m_userDetector->getSkeletonJointPosition(XN_SKEL_NECK);

	const float NECK_Y_ADJUSTMENT = 50; // adjust the neck position a little down
	pn.Y -= NECK_Y_ADJUSTMENT;

	m_vnh = pl1 - pn; // neck to hand
	m_ph = m_userDetector->getSkeletonJointPosition(XN_SKEL_HEAD);
	
	bool isHandCloseToNeck = m_vnh.magnitude2() < square(getNeckDistanceThreshold());
	bool isHandHorizontal = vsh.dotNormalized(vl) > getArmAngleThreshold();
	bool isOtherHandLow = pr1.Y < pl1.Y;

	return isHandCloseToNeck && isHandHorizontal && isOtherHandLow;
}

void EmeriumBeamDetector2::onPoseDetected(float dt)
{
	XV3 v(m_userDetector->getForwardVector());
	v.Y += m_vnh.Y / m_vnh.magnitude(); // adjust the vector as to the hand's position

	const float POSITION_Y_ADJUSTMENT = 70; // adjust the position a little up
	const float POSITION_FORWARD_ADJUSTMENT = 80; // adjust the position a little forward
	XV3 p(m_ph);
	p.Y += POSITION_Y_ADJUSTMENT;
	p += m_userDetector->getForwardVector() * POSITION_FORWARD_ADJUSTMENT;

	shootBeam(p, v);
}
