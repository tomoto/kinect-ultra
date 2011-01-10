#include "EyeSluggerRenderer.h"
#include "util.h"

const float MIN_BRIGHTNESS = 0.7f;
const float MAX_BRIGHTNESS = 2.0;

EyeSluggerRenderer::EyeSluggerRenderer(RenderingContext* rctx, HenshinDetector* henshinDetector) : AbstractOpenGLRenderer(rctx)
{
	m_henshinDetector = henshinDetector;
	m_shotLifeTime = 0;
	m_holdMode = HOLD_ON_HEAD;

	setupObjectModel();

	m_objectFrame.SetOrigin(0, 0, 0);
	m_objectFrame.SetForwardVector(0, 0, 1);
	m_objectFrame.SetUpVector(0, 1, 0);
}

EyeSluggerRenderer::~EyeSluggerRenderer()
{
}

void EyeSluggerRenderer::setupObjectModel()
{
	readBatchDef(getResourceFile("model", "eyeSlugger.vtx").c_str(), XV3(0, -140, -25), 1.1f, &m_batches);
}

XV3 EyeSluggerRenderer::getOrigin()
{
	return m_origin;
}

XV3 EyeSluggerRenderer::getForwardVector()
{
	M3DVector3f tmp;
	m_objectFrame.GetForwardVector(tmp);
	return XV3(tmp);
}

void EyeSluggerRenderer::setEnergy(float energy)
{
	m_brightness = interpolate(MIN_BRIGHTNESS, MAX_BRIGHTNESS, energy);
}

void EyeSluggerRenderer::setHoldMode(HoldMode holdMode)
{
	m_holdMode = holdMode;
}

bool EyeSluggerRenderer::isShot()
{
	return m_shotLifeTime > 0;
}

void EyeSluggerRenderer::shoot(const XV3& v, float rotation, int traceDencity)
{
	const float SHOT_LIFETIME = 2;

	m_shotVector = v;
	m_shotRotation = rotation;
	m_shotLifeTime = SHOT_LIFETIME;
	m_shotProgress = 0;
	m_shotTraceDencity = traceDencity;
}

void EyeSluggerRenderer::drawSlugger(float brightness, float alpha)
{
	const M3DVector3f LIGHT_POS =  { 0, 0, 0 };
	const M3DVector4f COLOR = { brightness, brightness, brightness, alpha };

	m_rctx->shaderMan->UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
		m_rctx->transform.GetModelViewMatrix(),
		m_rctx->transform.GetProjectionMatrix(),
		LIGHT_POS,
		COLOR
		);

	for (std::vector<GLBatch>::iterator b = m_batches.begin(); b != m_batches.end(); b++) {
		b->Draw();
	}
}

void EyeSluggerRenderer::drawHeld(float dt)
{
	switch (m_henshinDetector->getStage()) {
		case HenshinDetector::STAGE_HENSHINING:
		case HenshinDetector::STAGE_HENSHINED:
			break;
		default:
			return;
	}

	// TODO probably the position/orientation should be given from outside

	UserDetector* ud = m_henshinDetector->getUserDetector();
	XV3 p, fv, uv;
	bool updateObjectFrame;
	switch (m_holdMode) {
		case HOLD_ON_HEAD:
			{
				m_origin = ud->getSkeletonJointPosition(XN_SKEL_HEAD);
				fv = m_henshinDetector->getUserDetector()->getForwardVector();
				uv = m_henshinDetector->getUserDetector()->getUpVector();
				updateObjectFrame = true;
			}
			break;
		case HOLD_IN_HAND:
			{
				const float OFFSET = 200;
				m_origin = ud->getSkeletonJointPosition(XN_SKEL_RIGHT_HAND);
				fv = -(m_origin - ud->getSkeletonJointPosition(XN_SKEL_NECK));
				uv = fv.cross(ud->getSkeletonJointPosition(XN_SKEL_LEFT_SHOULDER) - ud->getSkeletonJointPosition(XN_SKEL_RIGHT_SHOULDER));
				uv.normalizeM();
				m_origin -= uv * OFFSET;
				updateObjectFrame = true;
			}
			break;
		default:
			updateObjectFrame = false;
			break;
	}

	if (updateObjectFrame) {
		m_objectFrame.SetOrigin(m_origin.X, m_origin.Y, m_origin.Z);
		m_objectFrame.SetForwardVector(fv.X, fv.Y, fv.Z);
		m_objectFrame.SetUpVector(uv.X, uv.Y, uv.Z);
		m_objectFrame.Normalize();
	}

	m_rctx->modelViewMatrix.PushMatrix();
	m_rctx->modelViewMatrix.MultMatrix(m_objectFrame);
	drawSlugger(m_brightness, 1);
	m_rctx->modelViewMatrix.PopMatrix();
}

void EyeSluggerRenderer::drawShot(float dt)
{
	// TODO should be position given from outside?
	UserDetector* ud = m_henshinDetector->getUserDetector();

	float rotationStep = float(M_PI) * 2 * m_shotRotation * dt / m_shotTraceDencity;
	float alphaStep = 1.0f / m_shotTraceDencity;
	float alpha = 0;

	for (int i = 0; i < m_shotTraceDencity; i++) {
		// set position
		XV3 p;
		if (m_shotLifeTime >= 1.0) {
			m_brightness = MAX_BRIGHTNESS;
			// do not change m_origin
			p = m_origin + m_shotVector * m_shotProgress;
		} else {
			m_brightness = MIN_BRIGHTNESS;
			m_origin = ud->getSkeletonJointPosition(XN_SKEL_HEAD);
			p = m_origin + m_henshinDetector->getUserDetector()->getForwardVector() * -m_shotLifeTime * m_shotVector.magnitude();
		}
		m_objectFrame.SetOrigin(p.X, p.Y, p.Z);
		
		// rotate
		m_objectFrame.RotateLocal(rotationStep, 1, 0, 0);

		// alpha
		alpha += alphaStep;

		m_rctx->modelViewMatrix.PushMatrix();
		m_rctx->modelViewMatrix.MultMatrix(m_objectFrame);
		drawSlugger(m_brightness, alpha);
		m_rctx->modelViewMatrix.PopMatrix();

		// progress
		m_shotLifeTime -= dt / m_shotTraceDencity;
		m_shotProgress += dt / m_shotTraceDencity;
		if (m_shotLifeTime <= 0) {
			m_shotLifeTime = 0;
			break; // terminate
		}
	}

}

void EyeSluggerRenderer::draw()
{
	float dt = m_ticker.tick();

	// TODO location should be given by detector?

	if (m_shotLifeTime > 0) {
		drawShot(dt);
	} else {
		drawHeld(dt);
	}
}
