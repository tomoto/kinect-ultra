#include "EmeriumBeamDetector2.h"
#include "util.h"

EmeriumBeamDetector2::EmeriumBeamDetector2(DepthGenerator* depthGen, UserDetector* userDetector, AbstractSimpleBeamRenderer* beamRenderer)
: AbstractEmeriumBeamDetector(depthGen, userDetector, beamRenderer)
{
}

EmeriumBeamDetector2::~EmeriumBeamDetector2()
{
}

bool EmeriumBeamDetector2::isPosing(float dt)
{
	UserGenerator* userGen = m_userDetector->getUserGenerator();

	XV3 pl0 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_ELBOW);
	XV3 pl1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_HAND);
	XV3 plsh = m_userDetector->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER);
	XV3 prsh = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER);
	XV3 pr1 = m_userDetector->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
	XV3 vl = pl1 - pl0; // left arm
	XV3 vsh = prsh - plsh; // shoulder line
	XV3 pn = m_userDetector->getSkeletonJointPosition(XN_SKEL_NECK);

	const float NECK_Y_ADJUSTMENT = 50; // adjust the neck position a little down
	pn.Y -= NECK_Y_ADJUSTMENT;

	m_vnh = pl1 - pn; // neck to hand
	m_ph = m_userDetector->getSkeletonJointPosition(XN_SKEL_HEAD);
	
	const float NECK_DISTANCE_THRESHOLD = 200.0f;
	const float ARM_ANGLE_THRESHOLD = 0.9f;

	bool isHandCloseToNeck = m_vnh.magnitude2() < square(NECK_DISTANCE_THRESHOLD);
	bool isHandHorizontal = vsh.dotNormalized(vl) > ARM_ANGLE_THRESHOLD;
	bool isOtherHandLow = pr1.Y < pl1.Y;

	return isHandCloseToNeck && isHandHorizontal && isOtherHandLow;
}

void EmeriumBeamDetector2::onPoseDetected(float dt)
{
	XV3 v(m_userDetector->getForwardVector());
	v.Y += m_vnh.Y / m_vnh.magnitude(); // adjust the vector as to the hand's position

	const float POSITION_Y_ADJUSTMENT = 70; // adjust the position a little up
	const float POSITION_FORWARD_ADJUSTMENT = 80; // adjust the position a little forward
	XV3 p(m_ph);
	p.Y += POSITION_Y_ADJUSTMENT;
	p += m_userDetector->getForwardVector() * POSITION_FORWARD_ADJUSTMENT;

	shootBeam(p, v);
}
