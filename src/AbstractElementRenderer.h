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

#ifndef _ABSTRACT_ELEMENT_RENDERER_H_
#define _ABSTRACT_ELEMENT_RENDERER_H_

#include "common.h"
#include "vec.h"
#include "AbstractOpenGLRenderer.h"
#include "TimeTicker.h"
#include <GLFrame.h>
#include <list>

struct AbstractElementRendererElement
{
	XV3 p; // position
	XV3 v; // forward vector
	float lifeTime; // life time left
};

template <class ElementType> class AbstractElementRenderer : public AbstractOpenGLRenderer
{
protected:
	GLuint m_textureID;
	TimeTicker m_ticker;
	std::list<ElementType> m_elements;

	float m_gravity;

public:
	AbstractElementRenderer(RenderingContext* rctx, const char* alphaTextureFile, float gravity)
		: AbstractOpenGLRenderer(rctx)
	{
		m_textureID = readAlphaTexture(alphaTextureFile);
		m_gravity = gravity;
	}

	virtual ~AbstractElementRenderer()
	{
	}

	virtual void draw()
	{
		executeDraw();

		float dt = m_ticker.tick();
		if (dt > 0.0f) {
			progress(dt);
		}
	}

protected:
	void setupTextureParameters()
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	virtual void executeDraw() = 0;

	void progress(float dt)
	{
		std::list<ElementType>::iterator i = m_elements.begin();
		while (i != m_elements.end()) {
			i->p += i->v * dt;
			i->v.Y -= m_gravity * dt;

			onProgress(&(*i), dt);

			i->lifeTime -= dt;
			if (i->lifeTime <= 0.0f) {
				i = m_elements.erase(i);
			} else {
				i++;
			}
		}
	}

	virtual void onProgress(ElementType* element, float dt)
	{
	}
};

#endif
