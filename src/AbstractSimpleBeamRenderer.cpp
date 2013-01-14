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
		f.RotateWorld(float(M_PI_2*(1+fabs(m_random.gaussian(0.2)))), e.d.X, e.d.Y, e.d.Z);
		f.RotateWorld(float(m_random.uniform(0.0, M_PI*2)), e.v.X, e.v.Y, e.v.Z);
		f.Normalize();

		M3DVector3f tmp;
		f.GetForwardVector(tmp);
		XV3 v(tmp);

		m_sparkRenderer->addElement(e.p, v * SPARK_VELOCITY);
	}
}
