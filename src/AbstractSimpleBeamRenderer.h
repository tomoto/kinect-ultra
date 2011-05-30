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

#ifndef _ABSTRACT_SIMPLE_BEAM_RENDERER_H_
#define _ABSTRACT_SIMPLE_BEAM_RENDERER_H_

#include "common.h"
#include "AbstractElementRenderer.h"
#include "VoxelObjectMapper.h"
#include "SparkRenderer.h"

struct AbstractSimpleBeamElement : public AbstractElementRendererElement
{
	XV3 d; // up vector
};

class AbstractSimpleBeamRenderer : public AbstractElementRenderer<AbstractSimpleBeamElement>
{
private:
	VoxelObjectMapper* m_voxelObjectMapper;
	SparkRenderer* m_sparkRenderer;
	float m_lifeTime;
	M3DVector4f m_color;
	int m_sparkDensity;
	cv::RNG m_random;

public:
	AbstractSimpleBeamRenderer(RenderingContext* rctx, VoxelObjectMapper* voxelObjectMapper, SparkRenderer* sparkRenderer,
		const char* alphaTextureFile, float lifeTime, const M3DVector4f color, int sparkDensity);
	virtual ~AbstractSimpleBeamRenderer();
	virtual void addElement(const XV3& p, const XV3& d, const XV3& v);

private:
	virtual void executeDraw();
	virtual void onProgress(AbstractSimpleBeamElement* element, float dt);
	void fireSpark(const AbstractSimpleBeamElement& element);
};

#endif
