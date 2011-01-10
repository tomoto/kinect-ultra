#ifndef _ABSTRACT_POSE_DETECTOR_H_
#define _ABSTRACT_POSE_DETECTOR_H_

#include "common.h"
#include "UserDetector.h"
#include "TimeTicker.h"

class AbstractPoseDetector
{
protected:
	UserDetector* m_userDetector;

private:
	// TODO: should be time instead of frame rate
	int m_requiredPosingStability;
	int m_posingTicks;

	TimeTicker m_ticker;

public:
	AbstractPoseDetector(UserDetector* userDetector);
	virtual ~AbstractPoseDetector();

	virtual void detect();

protected:
	void setRequiredPosingStability(int value) { m_requiredPosingStability = value; }

	virtual bool isPosing(float dt) = 0;
	virtual void onPoseDetected(float dt) = 0;
};

#endif
