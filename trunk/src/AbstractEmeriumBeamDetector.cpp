#include "AbstractEmeriumBeamDetector.h"
#include "util.h"

AbstractEmeriumBeamDetector::AbstractEmeriumBeamDetector(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer)
: AbstractPoseDetector(userDetector)
{
	m_beamRenderer = beamRenderer;
	setRequiredPosingStability(8);
}

AbstractEmeriumBeamDetector::~AbstractEmeriumBeamDetector()
{
}

void AbstractEmeriumBeamDetector::shootBeam(const XV3& p, const XV3& v)
{
	const float VELOCITY = 8000.0f;
	const float FORWARD_OFFSET = 20.0f;
	const float WIDTH = 30.0f;

	XV3 d0(v.Z, 0.0f, -v.X);
	d0.normalizeM();

	GLFrame frame;

	frame.SetForwardVector(XV3toM3D(v));
	frame.SetUpVector(XV3toM3D(d0));

	for (int i = 0; i < 4; i++) {
		M3DVector3f tmp;
		frame.GetUpVector(tmp);
		XV3 d(tmp);

		m_beamRenderer->addElement(p + v * FORWARD_OFFSET - d * WIDTH / 2.0f, d * WIDTH, v * VELOCITY);

		frame.RotateLocal(float(M_PI_4), 0.0f, 0.0f, 1.0f);
	}
}
