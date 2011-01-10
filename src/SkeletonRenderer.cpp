#include "SkeletonRenderer.h"
#include "util.h"

SkeletonRenderer::SkeletonRenderer(RenderingContext* rctx, DepthGenerator* depthGen, UserDetector* userDetector, HenshinDetector* henshinDetector)
: AbstractOpenGLRenderer(rctx)
{
	m_depthGen = depthGen;
	m_userDetector = userDetector;
	m_henshinDetector = henshinDetector;
}

SkeletonRenderer::~SkeletonRenderer()
{
}

void SkeletonRenderer::draw()
{
	if (m_henshinDetector->getStage() != HenshinDetector::STAGE_HENSHINED) {
		return;
	}

	XnUserID userID = m_userDetector->getTrackedUserID();
	if (!userID) {
		return;
	}

	glDisable(GL_DEPTH_TEST);
	glLineWidth(getPointSize()*2);
	glPointSize(getPointSize()*4);

	float color[] = { 0.7f, 0.7f, 0.7f, 0.7f };
	m_rctx->shaderMan->UseStockShader(GLT_SHADER_FLAT, m_rctx->transform.GetModelViewProjectionMatrix(), color);

	// drawBone(userID, XN_SKEL_NECK, XN_SKEL_HEAD);

	drawBone(userID, XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
	drawBone(userID, XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
	drawBone(userID, XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);

	drawBone(userID, XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
	drawBone(userID, XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
	drawBone(userID, XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

	drawBone(userID, XN_SKEL_NECK, XN_SKEL_TORSO);

	drawBone(userID, XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
	drawBone(userID, XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
	drawBone(userID, XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

	drawBone(userID, XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
	drawBone(userID, XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
	drawBone(userID, XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

	glEnable(GL_DEPTH_TEST);
}

void SkeletonRenderer::drawBone(XnUserID userID, XnSkeletonJoint fromJoint, XnSkeletonJoint toJoint)
{
	XnSkeletonJointPosition fromPos, toPos;
	UserGenerator* userGen = m_userDetector->getUserGenerator();
	userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, fromJoint, fromPos);
	userGen->GetSkeletonCap().GetSkeletonJointPosition(userID, toJoint, toPos);

	if (isConfident(fromPos) && isConfident(toPos)) {
		glBegin(GL_LINES);
		glVertex3f(fromPos.position.X, fromPos.position.Y, fromPos.position.Z);
		glVertex3f(toPos.position.X, toPos.position.Y, toPos.position.Z);
		glEnd();
	}

	if (isConfident(toPos)) {
		glBegin(GL_POINTS);
		glVertex3f(toPos.position.X, toPos.position.Y, toPos.position.Z);
		glEnd();
	}
}
