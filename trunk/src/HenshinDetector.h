#ifndef _HENSHIN_DETECTOR_H_
#define _HENSHIN_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "UserListener.h"
#include "UserDetector.h"
#include "TimeTicker.h"

class HenshinDetector : public AbstractPoseDetector, UserListener
{
public:
	enum Stage {
		STAGE_HUMAN,
		STAGE_HENSHINING,
		STAGE_HENSHINED,
		STAGE_DEHENSHINING
	};

private:
	UserDetector* m_userDetector;
	TimeTicker m_ticker;

	Stage m_stage;

public:
	HenshinDetector(UserDetector* userDetector);
	virtual ~HenshinDetector();

	UserDetector* getUserDetector() { return m_userDetector; }
	HenshinDetector::Stage getStage() { return m_stage; }

	void detect();

	float getHenshiningProgress();
	float getHenshinedTime();
	float getDehenshiningProgress();

	//
	// posing detector methods
	//

	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);

	//
	// user listener methods
	//

	virtual void onNewUser(XnUserID userID);
	virtual void onLostUser(XnUserID userID);
	virtual void onCalibrationStart(XnUserID userID);
	virtual void onCalibrationEnd(XnUserID userID, bool isSuccess);
	virtual void onPoseStart(XnUserID userID, const XnChar* pose);
	virtual void onPoseEnd(XnUserID userID, const XnChar* pose);

private:
	void transitToHuman();
	void transitToHenshining();
	void transitToHenshined();
	void transitToDehenshining();

	bool isDehenshinPosing();

};

#endif
