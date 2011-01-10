#include "WideshotDetector.h"
#include "util.h"

const float ARM_DISTANCE_THRESHOLD = 180.0f;
const float ARM_ANGLE_THRESHOLD = 0.25f;

WideshotDetector::WideshotDetector(DepthGenerator* depthGen, UserDetector* userDetector, WideshotRenderer* beamRenderer)
: AbstractPoseDetector(userDetector)
{
	m_beamRenderer = beamRenderer;
	setRequiredPosingStability(5);
}

WideshotDetector::~WideshotDetector()
{
}

bool WideshotDetector::isPosing(float dt)
{
	UserGenerator* userGen = m_userDetector->getUserGenerator();

	XV3 pr0 = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_ELBOW);
	XV3 pr1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
	XV3 prs = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER);
	XV3 pl0 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW);
	XV3 pl1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND);
	XV3 pls = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER);
	XV3 vr01(pr1 - pr0), vrs0(pr0 - prs);
	XV3 vl01(pl1 - pl0), vls0(pl0 - pls);

	m_pr0 = pr0;
	m_pr1 = pr1;
	m_vr01 = vr01;
	m_vl01 = vl01;

	bool areArmsClose = (pl1 - pr0).magnitude() < ARM_DISTANCE_THRESHOLD;
	bool isArmsOrthogonal = abs(vl01.dotNormalized(vr01)) < ARM_ANGLE_THRESHOLD;
	bool isRightArmOrthogonal = abs(vr01.dotNormalized(vrs0)) < ARM_ANGLE_THRESHOLD;
	bool isRightHandHigherThanLeft = pr1.Y > pl1.Y;

	return areArmsClose && isArmsOrthogonal && isRightArmOrthogonal && isRightHandHigherThanLeft;
}

void WideshotDetector::onPoseDetected(float dt)
{
	shootBeam(m_pr0, m_pr1, m_vr01.cross(m_vl01).normalize());
}

void WideshotDetector::shootBeam(const XV3& p0, const XV3& p1, const XV3& dv)
{
	XV3 p01(p1 - p0);
	XV3 dw(p01.cross(dv).normalize());
	const int DENCITY = 20;
	const float HEIGHT = 0.1f;
	const float VELOCITY = 5000.0f;
	const float FORWARD_DIFFUSION = 100.0f; // @ja 前後方向の分散
	const float HORIZONTAL_DIFFUSION = 30.0f; // @ja 左右方向の分散
	const float FORWARD_OFFSET = 100.0f;
	
	for (int i = 0; i < DENCITY; i++) {
		XV3 p(p0.interpolate(p1, m_random.uniform(0.0f, 1.0f - HEIGHT))); // 上下方向の分散
		p += dv * (FORWARD_OFFSET + m_random.uniform(0.0f, FORWARD_DIFFUSION));
		p += dw * float(m_random.gaussian(0.5)) * HORIZONTAL_DIFFUSION; 
		m_beamRenderer->addElement(p, p01 * HEIGHT, dv * VELOCITY);
	}
}
