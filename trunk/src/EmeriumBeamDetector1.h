#ifndef _EMERIUM_BEAM_DETECTOR1_H_
#define _EMERIUM_BEAM_DETECTOR1_H_

#include "common.h"
#include "AbstractEmeriumBeamDetector.h"

class EmeriumBeamDetector1 : public AbstractEmeriumBeamDetector
{
private:
	XV3 m_vr; // right arm vector
	XV3 m_vl; // left arm vector
	XV3 m_prl; // middle of hands

public:
	EmeriumBeamDetector1(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer);
	virtual ~EmeriumBeamDetector1();

private:
	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);
};

#endif
