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
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

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
