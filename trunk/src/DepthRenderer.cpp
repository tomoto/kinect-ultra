#include "DepthRenderer.h"

DepthRenderer::DepthRenderer(RenderingContext* rctx, DepthGenerator* depthGen) : AbstractTextureRenderer(rctx)
{
	m_depthGen = depthGen;

	DepthMetaData md;
	m_depthGen->GetMetaData(md);

	init(cv::Rect(md.XOffset(), md.YOffset(), md.XRes(), md.YRes()));
}

DepthRenderer::~DepthRenderer()
{
}

void DepthRenderer::setupCopy()
{
	DepthMetaData md;
	m_depthGen->GetMetaData(md);
	m_currentData = md.Data();
}

void DepthRenderer::copyRow(XnRGB24Pixel* dst, int srcOffset)
{
	for (int i = 0; i < m_imageRect.width; i++) {
		dst->nBlue = dst->nGreen = dst->nRed = 255 - *(m_currentData + srcOffset + i) / 16;
		dst++;
	}
}
