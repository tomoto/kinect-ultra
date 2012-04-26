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

#ifndef _EYE_SLUGGER_DETECTOR_EX_H_
#define _EYE_SLUGGER_DETECTOR_EX_H_

#include "common.h"
#include "EyeSluggerDetector.h"
#include "EyeSluggerRendererEx.h"

class EyeSluggerDetectorEx : public EyeSluggerDetector
{
public:
	enum UltraKnockStage {
		STAGE_NORMAL,
		STAGE_HOLDING_IN_HAND,
		STAGE_HOLDING_IN_HAND_POST,
		STAGE_HOLDING_IN_AIR
	};

private:
	EyeSluggerRendererEx* m_renderer;
	UltraKnockStage m_stage;

public:
	EyeSluggerDetectorEx(HenshinDetector* henshinDetector, EyeSluggerRendererEx* renderer);
	virtual ~EyeSluggerDetectorEx();

protected:
	virtual void onDetectPre(float dt);
	virtual void shootSlugger(float velocity, float rotation, int traceDencity);

	virtual void processUnposing(float dt);

private:
	bool isLeftArmStraightToFront();

	void holdSlugger();
	void fixSlugger();
	void restoreSlugger();

	void processPosingInHand(float dt);
	void processUnposingInHand(float dt);
	bool processPosesInHand(float dt);

	void processPosingInAir(float dt);
	void processUnposingInAir(float dt);
	bool processPosesInAir(float dt);
};

#endif
