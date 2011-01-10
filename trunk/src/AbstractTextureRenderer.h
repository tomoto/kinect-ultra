#ifndef _ABSTRACT_TEX_RENDERER_H_
#define _ABSTRACT_TEX_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include <GLShaderManager.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>

/**
 * Base class to render 2D image using texture
 */
class AbstractTextureRenderer : public AbstractOpenGLRenderer
{
protected:
	int m_textureWidth;
	int m_textureHeight;
	XnRGB24Pixel* m_textureData;
	cv::Rect m_imageRect;
	GLuint m_textureID;

	GLBatch m_batch;
	M3DMatrix44f m_orthoProjectionMatrix;
	bool m_isLocked;

public:
	AbstractTextureRenderer(RenderingContext* rctx);
	virtual ~AbstractTextureRenderer() = 0;
	virtual void draw();

	void lock(bool value);

protected:
	void init(const cv::Rect& imageRect);
	void setupBatch();

	// optionally overridable
	virtual void setupTexture();
	virtual void executeDraw();

	// need to override
	virtual void setupCopy() = 0;
	virtual void copyRow(XnRGB24Pixel* dst, int srcOffset) = 0;
};

#endif
