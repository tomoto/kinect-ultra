#include "ImageRenderer.h"
#include "config.h"

ImageRenderer::ImageRenderer(RenderingContext* rctx, ImageGenerator* imageGen) : AbstractTextureRenderer(rctx)
{
	m_imageGen = imageGen;

	ImageMetaData md;
	m_imageGen->GetMetaData(md);

	init(cv::Rect(md.XOffset(), md.YOffset(), md.XRes(), md.YRes()));
}

ImageRenderer::~ImageRenderer()
{
}

void ImageRenderer::setupCopy()
{
	ImageMetaData md;
	m_imageGen->GetMetaData(md);
	m_currentData = md.RGB24Data();
}

void ImageRenderer::copyRow(XnRGB24Pixel* dst, int srcOffset)
{
	xnOSMemCopy(dst, m_currentData + srcOffset, m_imageRect.width * sizeof(XnRGB24Pixel));
}
