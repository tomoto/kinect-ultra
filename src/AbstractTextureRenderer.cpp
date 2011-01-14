#include "AbstractTextureRenderer.h"

AbstractTextureRenderer::AbstractTextureRenderer(RenderingContext* rctx) : AbstractOpenGLRenderer(rctx), m_textureData(NULL)
{
}

AbstractTextureRenderer::~AbstractTextureRenderer()
{
	delete[] m_textureData;
}

void AbstractTextureRenderer::init(const cv::Rect& imageRect)
{
	m_textureWidth  = cvCeil(imageRect.width / 512.0) * 512;
	m_textureHeight = cvCeil(imageRect.height / 512.0) * 512;
	m_textureData = new XnRGB24Pixel[m_textureWidth * m_textureHeight];
	glGenTextures(1, &m_textureID);

	m_imageRect = imageRect;

	m3dLoadIdentity44(m_orthoProjectionMatrix);

	m_isLocked = FALSE;

	setupBatch();
}

void AbstractTextureRenderer::setupBatch()
{
	const float depth = 0.9999f;
	float rw = float(m_imageRect.width) / m_textureWidth; 
	float rh = float(m_imageRect.height) / m_textureHeight;

	m_batch.Begin(GL_QUADS, 4, 1);
	m_batch.MultiTexCoord2f(0, 0, 0);
	m_batch.Vertex3f(1.0f, 1.0f, depth);
	m_batch.MultiTexCoord2f(0, rw, 0);
	m_batch.Vertex3f(-1.0f, 1.0f, depth);
	m_batch.MultiTexCoord2f(0, rw, rh);
	m_batch.Vertex3f(-1.0f, -1.0f, depth);
	m_batch.MultiTexCoord2f(0, 0, rh);
	m_batch.Vertex3f(1.0f, -1.0f, depth);
	m_batch.End();
}

void AbstractTextureRenderer::draw()
{
	if (!m_isLocked) {
		setupTexture();
	}

	executeDraw();
}

void AbstractTextureRenderer::setupTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// clear the texture
	xnOSMemSet(m_textureData, 0, m_textureWidth * m_textureHeight * sizeof(XnRGB24Pixel));

	setupCopy();

	XnRGB24Pixel* dst = m_textureData + m_imageRect.x + m_imageRect.y * m_textureWidth;
	int srcOffset = 0;

	for (int i = 0; i < m_imageRect.height; i++) {
		copyRow(dst, srcOffset);
		dst += m_textureWidth;
		srcOffset += m_imageRect.width;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureWidth, m_textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_textureData);
}

void AbstractTextureRenderer::executeDraw()
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// If we want to do make this transparent... 
	//
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// float mod[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// m_rctx->shaderMan->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, m_projectionMatrix, mod, 0);

	m_rctx->shaderMan->UseStockShader(GLT_SHADER_TEXTURE_REPLACE, m_orthoProjectionMatrix, 0);
	m_batch.Draw();
}

void AbstractTextureRenderer::lock(bool value)
{
	m_isLocked = value;
}
