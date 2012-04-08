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
