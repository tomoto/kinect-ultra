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

#include "EmeriumBeamDetector1.h"
#include "util.h"

EmeriumBeamDetector1::EmeriumBeamDetector1(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer)
: AbstractEmeriumBeamDetector(depthGen, userDetector, beamRenderer)
{
}

EmeriumBeamDetector1::~EmeriumBeamDetector1()
{
}

float EmeriumBeamDetector1::getHeadDistanceThreshold()
{
	return 250.0f + 50.0f * getConfiguration()->getTriggerHappyLevel();
}

bool EmeriumBeamDetector1::isPosing(float dt)
{
	XnSkeletonJointPosition jr0, jr1, jl0,jl1;
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_ELBOW, &jr0);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND, &jr1);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW, &jl0);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND, &jl1);

	if (!isConfident(jr0) || !isConfident(jr1) || !isConfident(jl0) || !isConfident(jl1)) {
		return false;
	}

	XV3 pr0(jr0.position), pr1(jr1.position), pl0(jl0.position), pl1(jl1.position);

	m_vr = pr1 - pr0; // right arm
	m_vl = pl1 - pl0; // left arm
	m_prl = pr1.interpolate(pl1); // middle of hands

	XV3 ph = m_userDetector->getSkeletonJointPosition(XN_SKEL_HEAD);
	XV3 vhr(pr1 - ph), vhl(pl1 - ph), vhrl(m_prl - ph);

	vhr.Y *= 2;
	vhl.Y *= 2;

	bool areBothHandsCloseToHead = std::max(vhr.magnitude2() , vhl.magnitude2()) < square(getHeadDistanceThreshold());
	bool isArmAngleWide = m_vl.dot(m_vr) < 0;
	bool areHandsInFront = m_userDetector->getForwardVector().dot(vhrl) > 0;

	return areBothHandsCloseToHead && isArmAngleWide && areHandsInFront;
}

void EmeriumBeamDetector1::onPoseDetected(float dt)
{
	XV3 v(m_vr.cross(m_vl).normalize());

	const float VECTOR_Y_ADJUSTMENT = 0.2f; // adjust the angle a little up
	const float POSITION_Y_ADJUSTMENT = 20; // adjust the position a little up

	v.Y += VECTOR_Y_ADJUSTMENT;
	v.normalizeM();

	XV3 p(m_prl);
	p.Y += POSITION_Y_ADJUSTMENT;

	shootBeam(p, v);
}
