#ifndef _ABSTRACT_EMERIUM_BEAM_DETECTOR_H_
#define _ABSTRACT_EMERIUM_BEAM_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "AbstractSimpleBeamRenderer.h"

class AbstractEmeriumBeamDetector : public AbstractPoseDetector
{
private:
	AbstractSimpleBeamRenderer* m_beamRenderer;

public:
	AbstractEmeriumBeamDetector(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer);
	virtual ~AbstractEmeriumBeamDetector();

protected:
	void shootBeam(const XV3& p, const XV3& dv);
};

#endif
