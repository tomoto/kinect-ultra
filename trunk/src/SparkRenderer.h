#ifndef _SPARK_RENDERER_H_
#define _SPARK_RENDERER_H_

#include "common.h"
#include "AbstractElementRenderer.h"

struct SparkElement : public AbstractElementRendererElement
{
	XV3 d;
};

class SparkRenderer : public AbstractElementRenderer<SparkElement>
{
private:
	cv::RNG m_random;

public:
	SparkRenderer(RenderingContext* rctx);
	virtual ~SparkRenderer();
	virtual void addElement(const XV3& p, const XV3& v);

protected:
	void executeDraw();
};

#endif
