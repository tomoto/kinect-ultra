#ifndef _WIDESHOT_DETECTOR_H_
#define _WIDESHOT_DETECTOR_H_

#include "common.h"
#include "AbstractPoseDetector.h"
#include "WideshotRenderer.h"

class WideshotDetector : public AbstractPoseDetector
{
private:
	WideshotRenderer* m_beamRenderer;

	XV3 m_pr0, m_pr1, m_vr01, m_vl01;

	cv::RNG m_random;

public:
	WideshotDetector(DepthGenerator* depthGen, UserDetector* userDetector, WideshotRenderer* beamRenderer);
	virtual ~WideshotDetector();

private:
	virtual bool isPosing(float dt);
	virtual void onPoseDetected(float dt);

	void shootBeam(const XV3& p0, const XV3& p1, const XV3& dv);
};

#endif
