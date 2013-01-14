//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
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

#ifndef _USER_PROVIDER_H_
#define _USER_PROVIDER_H_

#include "common.h"
#include "AbstractSensorDataProvider.h"
#include "joint.h"

class UserProvider
{
public:
	UserProvider() {}
	virtual ~UserProvider() {}
	virtual XuUserID findFirstTrackedUserID() = 0;
	virtual XuUserID findTrackedUserIDNextTo(XuUserID userID) = 0;
	virtual bool isUserPositionTracked(XuUserID userID) = 0;
	virtual bool isUserSkeletonTracked(XuUserID userID) = 0;

	virtual void getSkeletonJointInfo(XuUserID userID, XuSkeletonJointIndex jointIndex, XuSkeletonJointInfo* pJointInfo) = 0;
};

#ifdef XU_KINECTSDK

class UserProviderImpl : public UserProvider, public AbstractSensorDataProvider {
private:
	NUI_SKELETON_FRAME m_frame;

public:
	UserProviderImpl(INuiSensor* pSensor);
	~UserProviderImpl();

	XuUserID findFirstTrackedUserID();
	XuUserID findTrackedUserIDNextTo(XuUserID userID);
	bool isUserPositionTracked(XuUserID userID);
	bool isUserSkeletonTracked(XuUserID userID);
	
	void getSkeletonJointInfo(XuUserID userID, XuSkeletonJointIndex jointIndex, XuSkeletonJointInfo* pJointInfo);

protected:
	virtual bool waitForNextFrameAndLockImpl(DWORD timeout);
	virtual void unlockImpl();

	const NUI_SKELETON_DATA* getSkeletonData(XuUserID userID);
};

#elif XU_OPENNI2

#include "DepthProvider.h"

class UserProviderImpl : public UserProvider, public AbstractSensorDataProvider {
private:
	DepthProviderImpl* m_depthProvider;

public:
	UserProviderImpl(DepthProviderImpl* depthProvider);
	~UserProviderImpl();

	nite::UserTracker* getUserTracker() { return m_depthProvider->getUserTracker(); }
	nite::UserTrackerFrameRef* getUserTrackerFrame() { return m_depthProvider->getUserTrackerFrame(); }

	XuUserID findFirstTrackedUserID();
	XuUserID findTrackedUserIDNextTo(XuUserID userID);
	bool isUserPositionTracked(XuUserID userID);
	bool isUserSkeletonTracked(XuUserID userID);
	
	void getSkeletonJointInfo(XuUserID userID, XuSkeletonJointIndex jointIndex, XuSkeletonJointInfo* pJointInfo);
};

#else // XU_OPENNI

class UserProviderImpl : public UserProvider, public AbstractSensorDataProvider {
private:
	UserGenerator m_userGen;

public:
	UserProviderImpl(Context* pContext);
	~UserProviderImpl();

	UserGenerator* getGenerator() { return &m_userGen; }

	XuUserID findFirstTrackedUserID();
	XuUserID findTrackedUserIDNextTo(XuUserID userID);
	bool isUserPositionTracked(XuUserID userID);
	bool isUserSkeletonTracked(XuUserID userID);
	
	void getSkeletonJointInfo(XuUserID userID, XuSkeletonJointIndex jointIndex, XuSkeletonJointInfo* pJointInfo);
};

#endif

#endif
