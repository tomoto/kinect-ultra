#include "SparkRenderer.h"
#include "util.h"

const float SPARK_RADIUS = 10.0f;
const float SPARK_LIFETIME = 0.5f;
const float SPARK_GRAVITY = 1000.0f;

SparkRenderer::SparkRenderer(RenderingContext* rctx) : AbstractElementRenderer(rctx, "sparkAlpha.png", SPARK_GRAVITY)
{
}

SparkRenderer::~SparkRenderer()
{
}

void SparkRenderer::addElement(const XV3& p, const XV3& v)
{
	SparkElement e;
	e.p = p;
	e.d = XV3(0, SPARK_RADIUS, 0); // TODO ?
	e.v = v;
	e.lifeTime = SPARK_LIFETIME;
	m_elements.push_back(e);
}

void SparkRenderer::executeDraw()
{
	setupTextureParameters();

	GLBatch b;
	b.Begin(GL_QUADS, m_elements.size() * 4, 1);

	for (std::list<SparkElement>::iterator i = m_elements.begin(); i != m_elements.end(); i++) {
		float color[] = { 1.0f, 1.0f, 0.9f, i->lifeTime / SPARK_LIFETIME };
		m_rctx->shaderMan->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, m_rctx->transform.GetModelViewProjectionMatrix(), color, 0);

		XV3 p = i->p;
		float r = i->d.magnitude();

		b.MultiTexCoord2f(0, 0, 0);
		b.Vertex3f(p.X - r, p.Y - r, p.Z);

		b.MultiTexCoord2f(0, 1, 0);
		b.Vertex3f(p.X + r, p.Y - r, p.Z);

		b.MultiTexCoord2f(0, 1, 1);
		b.Vertex3f(p.X + r, p.Y + r, p.Z);

		b.MultiTexCoord2f(0, 0, 1);
		b.Vertex3f(p.X - r, p.Y + r, p.Z);
	}

	b.End();
	b.Draw();
}
