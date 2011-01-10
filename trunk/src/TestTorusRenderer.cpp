#include "TestTorusRenderer.h"
#include "util.h"
#include "config.h"

TestTorusRenderer::TestTorusRenderer(RenderingContext* rctx)
{
	m_rctx = rctx;
	gltMakeTorus(m_batch, 2.0f, 0.5f, 32, 16);
}

TestTorusRenderer::~TestTorusRenderer()
{
}

void TestTorusRenderer::draw()
{
	m_rctx->modelViewMatrix.PushMatrix();
	{
		m_rctx->modelViewMatrix.MultMatrix(m_objectFrame);

		// antialiasing
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);

		// plane
		float planeColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		M3DVector4f lightPos =  { 5.0f, 50.0f, 5.0f, 1.0f };
		m_rctx->shaderMan->UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
			m_rctx->transform.GetModelViewMatrix(),
			m_rctx->transform.GetProjectionMatrix(),
			lightPos,
			planeColor);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glLineWidth(1.0f);
		glDisable(GL_POLYGON_OFFSET_LINE);
		m_batch.Draw();
	}
	m_rctx->modelViewMatrix.PopMatrix();

	m_objectFrame.RotateWorld(float(m3dDegToRad(1)), 1, 0.2f, 0);
}
