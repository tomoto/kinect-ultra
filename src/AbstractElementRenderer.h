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
