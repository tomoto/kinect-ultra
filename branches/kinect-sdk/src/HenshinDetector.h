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

#ifndef _HENSHIN_DETECTOR_H_
#define _HENSHIN_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "UserListener.h"
#include "UserDetector.h"
#include "Configurable.h"
#include "TimeTicker.h"

class HenshinDetector : public AbstractPoseDetector, UserListener, Configurable
{
public:
	enum Stage {
		STAGE_HUMAN,
		STAGE_HENSHINING,
		STAGE_HENSHINED,
		STAGE_DEHENSHINING
	};

private:
	TimeTicker m_ticker;
	Stage m_stage;

public:
	HenshinDetector(UserDetector* userDetector);
	virtual ~HenshinDetector();

	UserDetector* getUserDetector() { return m_userDetector; }
	HenshinDetector::Stage getStage() { return m_stage; }

	float getHenshiningProgress();
	float getHenshinedTime();
	float getDehenshiningProgress();

	//
	// posing detector methods
	//

	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);
	virtual void onDetectPost(float dt);

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
