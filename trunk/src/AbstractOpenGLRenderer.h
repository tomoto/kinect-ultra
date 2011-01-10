#ifndef _ABSTRACT_OPEN_GL_RENDERER_H_
#define _ABSTRACT_OPEN_GL_RENDERER_H_

#include "common.h"
#include "RenderingContext.h"

class AbstractOpenGLRenderer
{
protected:
	RenderingContext* m_rctx;

public:
	AbstractOpenGLRenderer(RenderingContext* rctx);
	virtual ~AbstractOpenGLRenderer() = 0;
};

#endif
