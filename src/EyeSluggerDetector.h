#ifndef _EYE_SLUGGER_DETECTOR_H_
#define _EYE_SLUGGER_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "EyeSluggerRenderer.h"

// TODO: separate to .cpp
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
	bool isPosingNomal(float dt);


	void fixSlugger();
	void posingInHand(float dt);
	void unposingInHand(float dt);
	bool isPosingInHand(float dt);

	void posingInAir(float dt);
	void unposingInAir(float dt);
	bool isPosingInAir(float dt);

	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);
};

#endif
