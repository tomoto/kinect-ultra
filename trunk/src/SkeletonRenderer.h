#ifndef _SKELETON_RENDERER_H_
#define _SKELETON_RENDERER_H_

#include "common.h"
#include "AbstractOpenGLRenderer.h"
#include "UserDetector.h"
#include "HenshinDetector.h"

class SkeletonRenderer : public AbstractOpenGLRenderer
{
private:
	DepthGenerator* m_depthGen;
	UserDetector* m_userDetector;
	HenshinDetector* m_henshinDetector;

public:
	SkeletonRenderer(RenderingContext* rctx, DepthGenerator* depthGen, UserDetector* userDetector, HenshinDetector* henshinDetector);
	virtual ~SkeletonRenderer();

	void draw();

private:
	void drawBone(XnUserID userID, XnSkeletonJoint fromJoint, XnSkeletonJoint toJoint);
};

#endif
