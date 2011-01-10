#include "AbstractSimpleBeamRenderer.h"
#include "util.h"

const float BEAM_LENGTH_FACTOR = 15; // beam length = velocity / this value
const float SPARK_VELOCITY = 4000;

AbstractSimpleBeamRenderer::AbstractSimpleBeamRenderer(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer,
													   const char* alphaTextureFile, float lifeTime, const M3DVector4f color, int sparkDensity)
: AbstractElementRenderer(rctx, alphaTextureFile, 0)
{
	m_voxelObjectMapper = voxelObjectMapper;
	m_sparkRenderer = sparkRenderer;

	m_lifeTime = lifeTime;
	m3dCopyVector4(m_color, color);
	m_sparkDensity = sparkDensity;
}

AbstractSimpleBeamRenderer::~AbstractSimpleBeamRenderer()
{
}

void AbstractSimpleBeamRenderer::addElement(const XV3& p, const XV3& d, const XV3& v)
{
	AbstractSimpleBeamElement e;
	e.p = p;
	e.v = v;
	e.d = d;
	e.lifeTime = m_lifeTime;
	m_elements.push_back(e);
}

void AbstractSimpleBeamRenderer::executeDraw()
{
	setupTextureParameters();

	m_rctx->shaderMan->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, m_rctx->transform.GetModelViewProjectionMatrix(), m_color, 0);

	// TODO probably I can use point texture
	GLBatch b;
	b.Begin(GL_QUADS, m_elements.size() * 4, 1);

	for (std::list<AbstractSimpleBeamElement>::iterator i = m_elements.begin(); i != m_elements.end(); i++) {
		XV3 p1 = i->p;
		XV3 p2 = p1 + i->v / BEAM_LENGTH_FACTOR;
		XV3 p3 = p2 + i->d;
		XV3 p4 = p1 + i->d;

		b.MultiTexCoord2f(0, 0, 0);
		b.Vertex3f(p1.X, p1.Y, p1.Z);

		b.MultiTexCoord2f(0, 1, 0);
		b.Vertex3f(p2.X, p2.Y, p2.Z);

		b.MultiTexCoord2f(0, 1, 1);
		b.Vertex3f(p3.X, p3.Y, p3.Z);

		b.MultiTexCoord2f(0, 0, 1);
		b.Vertex3f(p4.X, p4.Y, p4.Z);
	}

	b.End();
	b.Draw();
}

void AbstractSimpleBeamRenderer::onProgress(AbstractSimpleBeamElement* element, float dt)
{
	if (m_voxelObjectMapper->test(element->p, element->v * dt)) {
		fireSpark(*element);
		element->lifeTime = 0; // kill this beam
	}
}

void AbstractSimpleBeamRenderer::fireSpark(const AbstractSimpleBeamElement& e)
{
	for (int i = 0; i < m_sparkDensity; i++) {
		// TODO could be more elegant
		GLFrame f;
		f.SetForwardVector(e.v.X, e.v.Y, e.v.Z);
		f.RotateWorld(float(M_PI_2*(1+abs(m_random.gaussian(0.2)))), e.d.X, e.d.Y, e.d.Z);
		f.RotateWorld(float(m_random.uniform(0.0, M_PI*2)), e.v.X, e.v.Y, e.v.Z);
		f.Normalize();

		M3DVector3f tmp;
		f.GetForwardVector(tmp);
		XV3 v(tmp);

		m_sparkRenderer->addElement(e.p, v * SPARK_VELOCITY);
	}
}
