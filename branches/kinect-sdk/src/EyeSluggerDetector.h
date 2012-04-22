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

#ifndef _EYE_SLUGGER_DETECTOR_H_
#define _EYE_SLUGGER_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "EyeSluggerRenderer.h"

class EyeSluggerDetector : public AbstractPoseDetector
{
public:
	enum Stage {
		STAGE_NORMAL,
		STAGE_HOLDING_IN_HAND,
		STAGE_HOLDING_IN_HAND_POST,
		STAGE_HOLDING_IN_AIR
	};

private:
	HenshinDetector* m_henshinDetector;
	EyeSluggerRenderer* m_renderer;
	TimeTicker m_ticker;

	Stage m_stage;
	float m_energy;
	
public:
	EyeSluggerDetector(HenshinDetector* henshinDetector, EyeSluggerRenderer* renderer);
	virtual ~EyeSluggerDetector();

private:
	void enegize(float dt, float speed);
	void deenegize(float dt, float speed);
	void setEnergy(float e, bool delay = false);

	bool isLeftArmStraightToFront();
	bool isArmTopOfHead();
	void shootSlugger(float velocity, float rotation, int traceDencity);
	void holdSlugger();
	void restoreSlugger();

	void posingNormal(float dt);
	void unposingNormal(float dt);
	bool isHandBackOfHead();
	bool processPosingNomal(float dt);


	void fixSlugger();
	void posingInHand(float dt);
	void unposingInHand(float dt);
	bool processPosingInHand(float dt);

	void posingInAir(float dt);
	void unposingInAir(float dt);
	bool processPosingInAir(float dt);

	virtual void onDetectPre(float dt);
};

#endif
