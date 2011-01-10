#ifndef _EMERIUM_BEAM_DETECTOR2_H_
#define _EMERIUM_BEAM_DETECTOR2_H_

#include "common.h"
#include "AbstractEmeriumBeamDetector.h"

class EmeriumBeamDetector2 : public AbstractEmeriumBeamDetector
{
private:
	XV3 m_vnh; // neck to hand vector
	XV3 m_ph; // head position

public:
	EmeriumBeamDetector2(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer);
	virtual ~EmeriumBeamDetector2();

private:
	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);
};

#endif
