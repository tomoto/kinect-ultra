#ifndef _USER_DETECTOR_H_
#define _USER_DETECTOR_H_

#include "common.h"
#include "vec.h"
#include "UserListener.h"

class UserDetector : public UserListener
{
private:
	UserGenerator* m_userGen;
	XnChar* m_pose; // pose name for calibration, or null if posing is not required

	XnCallbackHandle m_hUserCallbacks;
	XnCallbackHandle m_hCalibrationCallbacks;
	XnCallbackHandle m_hPoseCallbacks;

	UserListener* m_listener;

	XnSkeletonJointPosition m_skeletonPositions[24];

public:
	UserDetector(UserGenerator* userGen);
	~UserDetector();

	UserGenerator* getUserGenerator() { return m_userGen; }

	XnUserID getTrackedUserID();

	bool isTrackedUserID(XnUserID userID)
	{
		return getTrackedUserID() == userID;
	}

	void stopTracking();

	void addListener(UserListener* listener);

	XV3 getSkeletonJointPosition(XnSkeletonJoint jointID);
	void getSkeletonJointPosition(XnSkeletonJoint eJoint, XnSkeletonJointPosition* pJointPosition);

	XV3 getForwardVector(); // returns the vector to the direction from the back to the front
	XV3 getUpVector(); // returns the vector to the body to the head

	// Listeners
	virtual void onNewUser(XnUserID userID);
	virtual void onLostUser(XnUserID userID);
	virtual void onCalibrationStart(XnUserID userID);
	virtual void onCalibrationEnd(XnUserID userID, bool isSuccess);
	virtual void onPoseStart(XnUserID userID, const XnChar* pose);
	virtual void onPoseEnd(XnUserID userID, const XnChar* pose);

private:
	void startCalibration(XnUserID userID);

};

#endif
