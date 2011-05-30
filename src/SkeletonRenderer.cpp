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
