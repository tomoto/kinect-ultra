#ifndef _DEPTH_RENDERER_H_
#define _DEPTH_RENDERER_H_

#include "common.h"
#include "AbstractTextureRenderer.h"

class DepthRenderer : public AbstractTextureRenderer
{
private:
	DepthGenerator* m_depthGen;
	const XnDepthPixel* m_currentData;

public:
	DepthRenderer(RenderingContext* rctx, DepthGenerator* depthGen);
	virtual ~DepthRenderer();

protected:
	virtual void setupCopy();
	virtual void copyRow(XnRGB24Pixel* dst, int srcOffset);

};

#endif
