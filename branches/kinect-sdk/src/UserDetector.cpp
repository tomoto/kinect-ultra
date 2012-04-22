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

#include "UserDetector.h"
#include "util.h"

static UserDetector* this_cast(void* that) {
	return static_cast<UserDetector*>(that);
}

UserDetector::UserDetector(UserProvider* userProvider) : m_listener(NULL), m_userProvider(userProvider), m_trackedUserID(0)
{
}

UserDetector::~UserDetector()
{
}

void UserDetector::detect()
{
	if (m_trackedUserID == 0) {
		XuUserID newUserID = m_userProvider->findFirstTrackedUserID();
		if (newUserID) {
			m_trackedUserID = newUserID;
			if (m_listener) {
				m_listener->onNewUser(newUserID);
			}
		}
	} else {
		if (!m_userProvider->isUserTracked(m_trackedUserID)) {
			if (m_listener) {
				m_listener->onLostUser(m_trackedUserID);
			}
			m_trackedUserID = 0;
		}
	}
}

XV3 UserDetector::getSkeletonJointPosition(XuSkeletonJointIndex jointIndex)
{
	XuSkeletonJointPosition j;
	getSkeletonJointPosition(jointIndex, &j);

	if (isConfident(j)) {
		m_skeletonPositions[jointIndex] = j;
	}
	return m_skeletonPositions[jointIndex].position;
}

void UserDetector::getSkeletonJointPosition(XuSkeletonJointIndex jointIndex, XuSkeletonJointPosition* pJointPosition)
{
	XuUserID userID = getTrackedUserID();
	if (userID) {
		m_userProvider->getSkeletonJointPosition(userID, jointIndex, pJointPosition);
	} else {
		pJointPosition->fConfidence = 0;
	}
}

void UserDetector::addListener(IUserListener* listener)
{
	assert(m_listener == NULL);
	m_listener = listener;
}

XV3 UserDetector::getForwardVector()
{
	XuUserID userID = getTrackedUserID();
	if (userID) {
		XV3 v0(getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER));
		XV3 v1(getSkeletonJointPosition(XN_SKEL_TORSO));
		XV3 v2(getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER));
		return ((v1 - v0).cross(v2 - v1)).normalize();
	} else {
		return XV3();
	}
}

XV3 UserDetector::getUpVector()
{
	XuUserID userID = getTrackedUserID();
	if (userID) {
		XV3 v0(getSkeletonJointPosition(XN_SKEL_TORSO));
		XV3 v1(getSkeletonJointPosition(XN_SKEL_NECK));
		return (v1 - v0).normalize();
	} else {
		return XV3();
	}
}
