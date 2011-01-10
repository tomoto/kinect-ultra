#ifndef _USER_LISTENER_H_
#define _USER_LISTENER_H_

#include "common.h"

class UserListener
{
public:
	virtual ~UserListener() = 0;
	virtual void onNewUser(XnUserID userID) = 0;
	virtual void onLostUser(XnUserID userID) = 0;
	virtual void onCalibrationStart(XnUserID userID) = 0;
	virtual void onCalibrationEnd(XnUserID userID, bool isSuccess) = 0;
	virtual void onPoseStart(XnUserID userID, const XnChar* pose) = 0;
	virtual void onPoseEnd(XnUserID userID, const XnChar* pose) = 0;
};

#endif
