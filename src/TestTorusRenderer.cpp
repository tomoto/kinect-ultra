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
