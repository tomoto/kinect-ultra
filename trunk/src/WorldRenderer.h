#ifndef _DEPTH_IMAGE_RENDERER_H_
#define _DEPTH_IMAGE_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include "HenshinDetector.h"
#include "TimeTicker.h"

class WorldRenderer : public AbstractOpenGLRenderer
{
protected:
	DepthGenerator* m_depthGen;
	ImageGenerator* m_imageGen;
	HenshinDetector* m_henshinDetector;

	M3DMatrix44f m_orthoProjectionMatrix;
	XnUInt32 m_width;
	XnUInt32 m_height;
	M3DVector3f* m_vertexBuf;
	M3DVector4f* m_colorBuf;

	GLuint m_henshinGlowTextureID;

	float m_depthAdjustment;

	bool m_tooLowAsHead;

public:
	WorldRenderer(RenderingContext* rctx, DepthGenerator* depthGen, ImageGenerator* imageGen, HenshinDetector* henshinDetector);
	virtual ~WorldRenderer();
	
	void draw();

	void addDepthAdjustment(float value) { m_depthAdjustment += value; }
private:
	XnUInt32 getNumPoints() { return m_width * m_height; }

	void getHenshinData(XnUserID* pUserID, const XnLabel** ppLabel, XV3* pHeadPoint, XV3* pNeckPoint, XnUInt32* pHenshinBottom, float* pFlyingOffset);

	void drawBackground();
	void drawHenshiningGlow();
};

#endif
