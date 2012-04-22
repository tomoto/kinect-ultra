//@COPYRIGHT@//

#include "UltraEyeRenderer.h"
#include "util.h"

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
	XV3 p(-0.95f, -0.95f, 0.0f), s(0.0009f, 0.002f, 1.0f);
	M3DVector4f color = { 0.0f, 0.7f, 0.0f, alpha };
	renderStrokeText(m_rctx, "Stand in front of the camera", p, s, 2.0f, color);
	glEnable(GL_DEPTH_TEST);
}

void UltraEyeRenderer::drawHenshinInstruction()
{
	XuSkeletonJointInfo jrh, jh;
	m_henshinDetector->getUserDetector()->getSkeletonJointInfo(XU_SKEL_RIGHT_HAND, &jrh);
	m_henshinDetector->getUserDetector()->getSkeletonJointInfo(XU_SKEL_HEAD, &jh);

	if (!isConfident(jrh) || !isConfident(jh)) return;

	XV3 fv(m_henshinDetector->getUserDetector()->getForwardVector());
	XV3 uv(m_henshinDetector->getUserDetector()->getUpVector());
	XV3 p1(jrh.position.interpolate(jh.position, 0.1f) + fv * 100);
	XV3 p2(jrh.position.interpolate(jh.position, 0.9f) + fv * 100 - uv * 50);
	float len = (p2 - p1).magnitude();
	XV3 p3(p1.interpolate(p2, 0.8f));
	XV3 v1(jrh.position - m_henshinDetector->getUserDetector()->getSkeletonJointPosition(XU_SKEL_RIGHT_ELBOW));
	XV3 v2 = (p2 - p1).cross(v1).normalize() * len * 0.1f;
	XV3 p4(p3 + v2);
	XV3 p5(p3 - v2);

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
	glVertex3f(p1.X, p1.Y, p1.Z);
	glVertex3f(p3.X, p3.Y, p3.Z);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(p2.X, p2.Y, p2.Z);
	glVertex3f(p4.X, p4.Y, p4.Z);
	glVertex3f(p5.X, p5.Y, p5.Z);
	glEnd();

	if (m_isNewUser) {
		XV3 p(-0.95f, -0.95f, 0.0f), s(0.001f, 0.002f, 1.0f);
		renderStrokeText(m_rctx, "Put your Ultra Eye On! Now!", p, s, 3.0f, arrowColor);
	}

	glEnable(GL_DEPTH_TEST);
}
