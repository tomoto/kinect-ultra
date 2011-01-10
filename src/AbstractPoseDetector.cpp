#include "AbstractPoseDetector.h"

AbstractPoseDetector::AbstractPoseDetector(UserDetector* userDetector)
{
	m_userDetector = userDetector;

	m_requiredPosingStability = 0;
	m_posingTicks = 0;
}

AbstractPoseDetector::~AbstractPoseDetector()
{
}

void AbstractPoseDetector::detect()
{
	float dt = m_ticker.tick();

	XnUserID userID = m_userDetector->getTrackedUserID();
	if (!userID) {
		return;
	}

	// TODO: should be time instead of frame rate
	if (isPosing(dt)) {
		if (m_posingTicks < m_requiredPosingStability) {
			m_posingTicks++;
		}
		if (m_posingTicks >= m_requiredPosingStability) {
			onPoseDetected(dt);
		}
	} else {
		if (m_posingTicks > 0) {
			m_posingTicks--;
		}
	}
}
