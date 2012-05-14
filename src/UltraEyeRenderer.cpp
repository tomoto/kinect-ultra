//@COPYRIGHT@//
//
// Copyright (c) 2012, Tomoto S. Washio
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

#include "UltraEyeRenderer.h"
#include "util.h"
#include <cmath>

UltraEyeRenderer::UltraEyeRenderer(RenderingContext* rctx, HenshinDetector* henshinDetector) : AbstractOpenGLRenderer(rctx)
{
	m_henshinDetector = henshinDetector;
	m_isNewUser = true;
	m_phase = 0;
}

UltraEyeRenderer::~UltraEyeRenderer()
{
}

void UltraEyeRenderer::draw()
{
	if (!m_henshinDetector->getUserDetector()->getTrackedUserID()) {
		drawSearchingInstruction();
		m_isNewUser = true;
		return;
	}

	if (m_henshinDetector->getStage() != HenshinDetector::STAGE_HUMAN) {
		m_isNewUser = false;
		return;
	}

	drawHenshinInstruction();
}

void UltraEyeRenderer::drawSearchingInstruction()
{
	m_phase += m_ticker.tick() * 1.5f;
	float alpha = square(std::sin(m_phase));

	glDisable(GL_DEPTH_TEST);
	XV3 p(-0.95f, 0.80f, 0.0f), s(0.0009f, 0.0015f, 1.0f);
	M3DVector4f color = { 0.0f, 0.7f, 0.0f, alpha };
	renderStrokeText(m_rctx, "Stand in front of the camera", p, s, 2.0f, color);
	glEnable(GL_DEPTH_TEST);
}

void UltraEyeRenderer::drawHenshinInstruction()
{
	XuSkeletonJointInfo jrh, jh, jre;
	m_henshinDetector->getUserDetector()->getSkeletonJointInfo(XU_SKEL_RIGHT_HAND, &jrh);
	m_henshinDetector->getUserDetector()->getSkeletonJointInfo(XU_SKEL_HEAD, &jh);
	m_henshinDetector->getUserDetector()->getSkeletonJointInfo(XU_SKEL_RIGHT_ELBOW, &jre);

	if (!isConfident(jrh) || !isConfident(jh) || !isConfident(jre)) return;

	const float CIRCLE_RADIUS = 100;

	XV3 fv(m_henshinDetector->getUserDetector()->getForwardVector());
	XV3 uv(m_henshinDetector->getUserDetector()->getUpVector());
	XV3 armDirection((jrh.position - jre.position).normalize());
	XV3 adjustedRightHand(jrh.position + armDirection * 30); // slightly move to the fingertip side
	XV3 adjustedHead(jh.position + fv * 100); // slightly move forward
	XV3 arrowTip(adjustedRightHand.interpolate(adjustedHead, 0.95f));
	XV3 arrowBottom(adjustedRightHand.interpolate(adjustedHead, 0.0f));
	float len = (arrowTip - arrowBottom).magnitude();
	XV3 triangleBottom(arrowBottom.interpolate(arrowTip, 0.8f));
	XV3 triangleOpening = (arrowTip - arrowBottom).cross(armDirection).normalize() * len * 0.1f;
	XV3 arrowPlaneNorm = (arrowTip - arrowBottom).cross(triangleOpening).normalize();
	XV3 triangleEnd1(triangleBottom + triangleOpening);
	XV3 triangleEnd2(triangleBottom - triangleOpening);

	float maxAlpha = cramp((len - 50.0f) / 150.0f, 0, 1);
	float blinkSpeed = 1000.0f / std::max(len - 100.0f, 100.f);
	m_phase += m_ticker.tick() * blinkSpeed;
	float alpha = square(std::sin(m_phase)) * maxAlpha;

	M3DVector4f arrowColor = { 0.7f, 0.0f, 0.0f, alpha };
	m_rctx->shaderMan->UseStockShader(GLT_SHADER_FLAT, m_rctx->transform.GetModelViewProjectionMatrix(), arrowColor);

	const float THICKNESS = 2;
	glDisable(GL_DEPTH_TEST);
	glLineWidth(getPointSize() * THICKNESS);
	glPointSize(getPointSize() * THICKNESS);

	glBegin(GL_LINES);
	if (len > CIRCLE_RADIUS) {
		glVertex3fv(XV3toM3D(arrowBottom + (arrowTip - arrowBottom).normalize() * CIRCLE_RADIUS));
		glVertex3fv(XV3toM3D(arrowTip));
	}
	glVertex3fv(XV3toM3D(arrowTip));
	glVertex3fv(XV3toM3D(triangleEnd1));
	glVertex3fv(XV3toM3D(arrowTip));
	glVertex3fv(XV3toM3D(triangleEnd2));
	glEnd();

	glBegin(GL_LINE_LOOP);
	XV3 r0((arrowTip - arrowBottom).normalize() * CIRCLE_RADIUS);
	GLFrame f;
	f.SetForwardVector(0, 0, 1); // invert Z
	const int SEGMENTS = 24;
	const float STEP_ANGLE = float(M_PI * 2 / SEGMENTS);
	for (int i = 0; i < SEGMENTS; i++) {
		f.RotateLocal(STEP_ANGLE, arrowPlaneNorm.X, arrowPlaneNorm.Y, arrowPlaneNorm.Z);
		M3DVector3f r;
		f.TransformPoint(XV3toM3D(r0), r);
		glVertex3fv(XV3toM3D(arrowBottom + r));
	}
	glEnd();

	if (m_isNewUser) {
		XV3 p(-0.95f, 0.80f, 0.0f), s(0.001f, 0.0015f, 1.0f);
		renderStrokeText(m_rctx, "Put your Ultra Eye On! Now!", p, s, 3.0f, arrowColor);
	}

	glEnable(GL_DEPTH_TEST);
}
