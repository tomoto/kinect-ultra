#include "HenshinDetector.h"
#include "util.h"

HenshinDetector::HenshinDetector(UserDetector* userDetector) : AbstractPoseDetector(userDetector)
{
	m_userDetector = userDetector;
	m_userDetector->addListener(this);
	setRequiredPosingStability(5);
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

	const float STRAIGHT_THRESHOLD = 0.9f;
	return vrse.dot(vreh) > STRAIGHT_THRESHOLD && vrse.dot(vleh) > STRAIGHT_THRESHOLD &&
		vrse.Y > STRAIGHT_THRESHOLD && vlse.Y > STRAIGHT_THRESHOLD;
}

void HenshinDetector::detect()
{
	AbstractPoseDetector::detect();

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
