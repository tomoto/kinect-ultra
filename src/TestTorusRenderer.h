#ifndef _TEST_TORUS_RENDERER_H_
#define _TEST_TORUS_RENDERER_H_

#include "common.h"
#include "RenderingContext.h"

class TestTorusRenderer
{
private:
	RenderingContext* m_rctx;
	GLTriangleBatch m_batch;
	
	GLFrame m_objectFrame;

public:
	TestTorusRenderer(RenderingContext* rctx);
	virtual ~TestTorusRenderer();

	void draw();
};

#endif
