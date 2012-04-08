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

#include "HenshinDetector.h"
#include "util.h"

HenshinDetector::HenshinDetector(UserDetector* userDetector) : AbstractPoseDetector(userDetector)
{
	m_userDetector->addListener(this);
	setRequiredPosingStability(0.15f);
	transitToHuman();
}

HenshinDetector::~HenshinDetector()
{
}

void HenshinDetector::transitToHuman()
{
	printf("Henshin Stage = HUMAN\n");
	m_stage = STAGE_HUMAN;
	m_ticker.unlock();
	m_userDetector->stopTracking();
}

void HenshinDetector::transitToHenshining()
{
	printf("Henshin Stage = HENSHINING\n");
	m_stage = STAGE_HENSHINING;
	m_ticker.lock();
}

void HenshinDetector::transitToHenshined()
{
	printf("Henshin Stage = HENSHINED\n");
	m_stage = STAGE_HENSHINED;
	m_ticker.lock();
}


void HenshinDetector::transitToDehenshining()
{
	printf("Henshin Stage = DEHENSHINING\n");
	m_stage = STAGE_DEHENSHINING;
	m_ticker.lock();
}

bool HenshinDetector::isDehenshinPosing()
{
	XnSkeletonJointPosition jrs, jre, jrh, jls, jle, jlh;
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER, &jrs);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_ELBOW, &jre);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND, &jrh);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER, &jls);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW, &jle);
	m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND, &jlh);

	if (!isConfident(jrs) || !isConfident(jre) || !isConfident(jrh) ||
			!isConfident(jls) || !isConfident(jle) || !isConfident(jlh)) {
		return false;
	}

	XV3 prs(jrs.position), pre(jre.position), prh(jrh.position);
	XV3 pls(jls.position), ple(jle.position), plh(jlh.position);
	XV3 vrse((pre - prs).normalize()), vreh((prh - pre).normalize());
	XV3 vlse((ple - pls).normalize()), vleh((plh - ple).normalize());
	XV3 up(0, 1, 0);

	const float STRAIGHT_THRESHOLD = 0.85f - 0.1f * getConfiguration()->getTriggerHappyLevel();
	return vrse.dot(vreh) > STRAIGHT_THRESHOLD && vrse.dot(vleh) > STRAIGHT_THRESHOLD &&
		vrse.Y > STRAIGHT_THRESHOLD && vlse.Y > STRAIGHT_THRESHOLD;
}

void HenshinDetector::onDetectPost(float dt)
{
	switch (m_stage) {
		case STAGE_HENSHINING:
			if (getHenshiningProgress() > 1) {
				transitToHenshined();
			}
			break;
		case STAGE_HENSHINED:
			if (getHenshinedTime() > 180) {
				transitToHuman();
			}
			break;
		case STAGE_DEHENSHINING:
			if (getDehenshiningProgress() > 1.0) {
				transitToHuman();
			}
			break;
	}
}

float HenshinDetector::getHenshiningProgress()
{
	const float HENSHINING_DURATION = 3;
	return m_stage == STAGE_HENSHINING ? m_ticker.tick() / HENSHINING_DURATION : 0; }

float HenshinDetector::getHenshinedTime()
{
	return m_stage == STAGE_HENSHINED ? m_ticker.tick() : 0;
}

float HenshinDetector::getDehenshiningProgress()
{
	const float DEHENSHINING_DURATION = 3;
	return m_stage == STAGE_DEHENSHINING ? m_ticker.tick() / DEHENSHINING_DURATION : 0;
}

//
// posing detector methods
//


bool HenshinDetector::isPosing(float dt)
{
	return isDehenshinPosing();
}

void HenshinDetector::onPoseDetected(float dt)
{
	if (m_stage == STAGE_HENSHINED) {
		transitToDehenshining();
	}
}

//
// listener methods
//

void HenshinDetector::onNewUser(XnUserID userID)
{
}

void HenshinDetector::onLostUser(XnUserID userID)
{
	if (m_userDetector->isTrackedUserID(userID)) {
		transitToHuman();
	}
}

void HenshinDetector::onCalibrationStart(XnUserID userID)
{
}

void HenshinDetector::onCalibrationEnd(XnUserID userID, bool isSuccess)
{
	if (isSuccess) {
		transitToHenshining();
	} else {
		transitToHuman();
	}
}

void HenshinDetector::onPoseStart(XnUserID userID, const XnChar* pose)
{
}

void HenshinDetector::onPoseEnd(XnUserID userID, const XnChar* pose)
{
}
