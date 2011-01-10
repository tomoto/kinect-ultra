#ifndef _IMAGE_RENDERER_H_
#define _IMAGE_RENDERER_H_

#include "common.h"
#include "AbstractTextureRenderer.h"

class ImageRenderer : public AbstractTextureRenderer
{
private:
	ImageGenerator* m_imageGen;
	const XnRGB24Pixel* m_currentData;

public:
	ImageRenderer(RenderingContext* rctx, ImageGenerator* imageGen);
	virtual ~ImageRenderer();

protected:
	virtual void setupCopy();
	virtual void copyRow(XnRGB24Pixel* dst, int srcOffset);

};

#endif
