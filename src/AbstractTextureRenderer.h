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

#ifndef _ABSTRACT_TEX_RENDERER_H_
#define _ABSTRACT_TEX_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include <GLShaderManager.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>

/**
 * Base class to render 2D image using texture
 */
class AbstractTextureRenderer : public AbstractOpenGLRenderer
{
protected:
	int m_textureWidth;
	int m_textureHeight;
	XnRGB24Pixel* m_textureData;
	cv::Rect m_imageRect;
	GLuint m_textureID;

	GLBatch m_batch;
	M3DMatrix44f m_orthoProjectionMatrix;
	bool m_isLocked;

public:
	AbstractTextureRenderer(RenderingContext* rctx);
	virtual ~AbstractTextureRenderer() = 0;
	virtual void draw();

	void lock(bool value);

protected:
	void init(const cv::Rect& imageRect);
	void setupBatch();

	// optionally overridable
	virtual void setupTexture();
	virtual void executeDraw();

	// need to override
	virtual void setupCopy() = 0;
	virtual void copyRow(XnRGB24Pixel* dst, int srcOffset) = 0;
};

#endif
