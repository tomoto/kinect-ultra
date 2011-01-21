//@COPYRIGHT@//
//
// Copyright (c) 2011, Tomoto S. Washio
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Tomoto S. Washio nor the names of his
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#include "WorldRenderer.h"
#include "config.h"
#include <GLFrustum.h>
#include "util.h"

const float DEFAULT_DEPTH_ADJUSTMENT = 105; // empirical valuel. adjustable by 'q' and 'a' key.

WorldRenderer::WorldRenderer(RenderingContext* rctx, DepthGenerator* depthGen, ImageGenerator* imageGen, HenshinDetector* henshinDetector)
: AbstractOpenGLRenderer(rctx)
{
	m_depthGen = depthGen;
	m_imageGen = imageGen;
	m_henshinDetector = henshinDetector;

	DepthMetaData dmd;
	m_depthGen->GetMetaData(dmd);
	m_width = dmd.XRes();
	m_height = dmd.YRes();

	// allocate working buffers
	XnUInt32 numPoints = getNumPoints();
	m_vertexBuf = new M3DVector3f[numPoints];
	m_colorBuf = new M3DVector4f[numPoints];

	// pre-set values on working buffers
	M3DVector3f* vp = m_vertexBuf;
	M3DVector4f* cp = m_colorBuf;
	for (XnUInt32 iy = 0; iy < m_height; iy++) {
		float y = 1.0f - iy * 2.0f / m_height;
		for (XnUInt32 ix = 0; ix < m_width; ix++) {
			float x = 1.0f - ix * 2.0f / m_width;
			(*vp)[0] = x;
			(*vp)[1] = (*vp)[2] = 0;
			vp++;
			(*cp)[0] = (*cp)[1] = (*cp)[2] = 0;
			(*cp)[3] = 1; // alpha is always 1.0
			cp++;
		}
	}

	m_batch.init(numPoints);

	// texture for henshin glow
	m_henshinGlowTextureID = readAlphaTexture("sparkAlpha.png");

	m_depthAdjustment = DEFAULT_DEPTH_ADJUSTMENT;
}

WorldRenderer::~WorldRenderer()
{
	delete[] m_vertexBuf;
	delete[] m_colorBuf;
}

void WorldRenderer::getHenshinData(XnUserID* pUserID, const XnLabel** ppLabel, XV3* pHeadPoint, XV3* pNeckPoint, XnUInt32* pHenshinBottom, float* pFlyingOffset)
{
	UserDetector* userDetector = m_henshinDetector->getUserDetector();

	if (m_henshinDetector->getStage() != HenshinDetector::STAGE_HUMAN) {
		XnUserID userID = *pUserID = userDetector->getTrackedUserID();
		SceneMetaData smd;
		userDetector->getUserGenerator()->GetUserPixels(userID, smd);

		*ppLabel = smd.Data();

		XnSkeletonJointPosition headPos, neckPos;
		userDetector->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_HEAD, headPos);
		userDetector->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_NECK, neckPos);

		// if (isConfident(headPos) && isConfident(neckPos)) {
			// some adjustment
			*pHeadPoint = headPos.position;
			pHeadPoint->interpolateM(neckPos.position, -0.2f); // a little up
			*pNeckPoint = headPos.position;
			pNeckPoint->interpolateM(neckPos.position, 0.5f); // middle of head and neck
		// }

		switch (m_henshinDetector->getStage()) {
			case HenshinDetector::STAGE_HENSHINING:
				{
					const int HENSHIN_STEPS = 15;
					XnPoint3D pp;
					m_depthGen->ConvertRealWorldToProjective(1, &headPos.position, &pp);
					float t = m_henshinDetector->getHenshiningProgress();
					t = floor(t * t * HENSHIN_STEPS) / HENSHIN_STEPS;
					*pHenshinBottom = (XnUInt32)(pp.Y + t * m_height / 2);
				}
				break;
			case HenshinDetector::STAGE_HENSHINED:
				{
					float ht = m_henshinDetector->getHenshinedTime();

					// blink later than 150 secs being henshined
					*pHenshinBottom = (ht < 150.0f || ht - floor(ht) < 0.5f) ? m_height : 0;
				}
				break;
			case HenshinDetector::STAGE_DEHENSHINING:
				{
					const int DEHENSHIN_FLYING_VELOCITY = 12;
					*pFlyingOffset = m_henshinDetector->getDehenshiningProgress() * DEHENSHIN_FLYING_VELOCITY;
					*pHenshinBottom = m_height;
				}
				break;
		}
	}
}

#ifndef USE_MACRO
inline void setGray(M3DVector4f* cp, float g)
{
	(*cp)[0] = (*cp)[1] = (*cp)[2] = g * 0.7f + 0.2f;
}
#else
#define setGray(cp, g) ((*(cp))[0] = (*(cp))[1] = (*(cp))[2] = (g) * 0.7f + 0.2f)
#endif

#ifndef USE_MACRO
inline void setRed(M3DVector4f* cp, float g)
{
	(*cp)[0] = g * 0.8f + 0.2f;
	(*cp)[1] = (*cp)[2] = 0;
}
#else
#define setRed(cp, g) ((*(cp))[0] = (g) * 0.8f + 0.2f, (*(cp))[1] = (*(cp))[2] = 0)
#endif

#ifndef USE_MACRO
inline void setRGB(M3DVector4f* cp, const XnRGB24Pixel& p)
{
	(*cp)[0] = b2fNormalized(p.nRed);
	(*cp)[1] = b2fNormalized(p.nGreen);
	(*cp)[2] = b2fNormalized(p.nBlue);
}
#else
#define setRGB(cp, p) ((*(cp))[0] = b2fNormalized((p).nRed), (*(cp))[1] = b2fNormalized((p).nGreen), (*(cp))[2] = b2fNormalized((p).nBlue))
#endif

void WorldRenderer::draw()
{
	drawBackground();

	if (m_henshinDetector->getStage() == HenshinDetector::STAGE_HENSHINING) {
		drawHenshiningGlow();
	}
}

void WorldRenderer::drawHenshiningGlow()
{
	// TODO how can I avoid the glow to hide 3D objects behind?

	UserDetector* userDetector = m_henshinDetector->getUserDetector();

	XnUserID userID = userDetector->getTrackedUserID();
	if (!userID) {
		return;
	}

	XnSkeletonJointPosition headPos;
	userDetector->getUserGenerator()->GetSkeletonCap().GetSkeletonJointPosition(userID, XN_SKEL_HEAD, headPos);
	if (!isConfident(headPos)) {
		return;
	}
	XV3 p(headPos.position);

	float t = m_henshinDetector->getHenshiningProgress();

	M3DVector4f color = { 1.0f, 1.0f, 1.0f, 1.0f-abs(t-0.5f)*2 };
	m_rctx->shaderMan->UseStockShader(GLT_SHADER_TEXTURE_MODULATE, m_rctx->transform.GetModelViewProjectionMatrix(), color, 0);

	glBindTexture(GL_TEXTURE_2D, m_henshinGlowTextureID);

	const float r = 600.0f; // radius
	const float zOffset = 50.0f; // depth adjustment

	GLBatch b;
	b.Begin(GL_QUADS, 4, 1);

	b.MultiTexCoord2f(0, 0, 0);
	b.Vertex3f(p.X - r, p.Y - r, p.Z + zOffset);

	b.MultiTexCoord2f(0, 1, 0);
	b.Vertex3f(p.X + r, p.Y - r, p.Z + zOffset);

	b.MultiTexCoord2f(0, 1, 1);
	b.Vertex3f(p.X + r, p.Y + r, p.Z + zOffset);

	b.MultiTexCoord2f(0, 0, 1);
	b.Vertex3f(p.X - r, p.Y + r, p.Z + zOffset);

	b.End();
	b.Draw();
}

void WorldRenderer::drawBackground()
{
	// setup shader
	m_rctx->shaderMan->UseStockShader(GLT_SHADER_SHADED, m_rctx->orthoMatrix.GetMatrix());

	// get depth buffer
	DepthMetaData dmd;
	m_depthGen->GetMetaData(dmd);
	const XnDepthPixel* dp = dmd.Data();

	// get image buffer
	ImageMetaData imd;
	m_imageGen->GetMetaData(imd);
	const XnRGB24Pixel* ip = imd.RGB24Data();

	// get henshin-related information
	XnUserID userID = 0;
	const XnLabel* lp = NULL;
	XV3 headPoint, neckPoint;
	XnUInt32 henshinBottom = 0;
	float flyingOffset = 0;
	getHenshinData(&userID, &lp, &headPoint, &neckPoint, &henshinBottom, &flyingOffset);

	// get working buffers
	M3DVector3f* vp = m_vertexBuf;
	M3DVector4f* cp = m_colorBuf;

	XnUInt32 numPoints = getNumPoints();

	bool tooLowAsHead = false; // for head-detection optimization

	const float HEAD_AREA_BOTTOM = 300;
	const float HEAD_AREA_DIST = 148;
	const float NECK_AREA_DIST = 148;
	const float HEAD_AREA_DIST2 = square(HEAD_AREA_DIST);
	const float NECK_AREA_DIST2 = square(NECK_AREA_DIST);

	glEnable(GL_POINT_SIZE);
	glPointSize(getPointSize());
	// glBegin(GL_POINTS);

	XnUInt32 ix = 0, iy = 0;
	float y = 0;
	float nearZ = PERSPECTIVE_Z_MIN + m_depthAdjustment;
	for (XnUInt32 i = 0; i < numPoints; i++, dp++, ip++, vp++, cp++, lp++, ix++) {

		if (ix == m_width) {
			ix = 0;
			iy++;
			y = 1.0f - iy * 2.0f / m_height;
		}

		if (userID && *lp == userID && iy < henshinBottom) {
			// paint in henshin color
			float g = b2fNormalized(convertRGBtoGray(ip->nRed, ip->nGreen, ip->nBlue));
			if (tooLowAsHead) {
				setRed(cp, g); // head-detection optimization
			} else {
				XV3 pp(float(ix), float(iy), *dp);
				XV3 rp;
				m_depthGen->ConvertProjectiveToRealWorld(1, &pp, &rp);
				if (headPoint.Y - rp.Y < HEAD_AREA_BOTTOM) {
					if (headPoint.distance2(rp) < HEAD_AREA_DIST2 || neckPoint.distance2(rp) < NECK_AREA_DIST2) {
						setGray(cp , g);
					} else {
						setRed(cp, g);
					}
				} else {
					// too low as head, no longer necessary to check the distance from head and neck
					tooLowAsHead = true;
					setRed(cp, g);
				}
			}

			// (*vp)[0] (x) is already set
			(*vp)[1] = y + flyingOffset;
		} else {
			setRGB(cp, *ip);

			// (*vp)[0] (x) is already set
			(*vp)[1] = y;
		}
		
		(*vp)[2] = (*dp) ? getNormalizedDepth(*dp, nearZ, PERSPECTIVE_Z_MAX) : Z_INFINITE;

		// glVertexAttrib3fv(GLT_ATTRIBUTE_VERTEX, (float*)vp);
		// glVertexAttrib4fv(GLT_ATTRIBUTE_COLOR, (float*)cp);
	}
	// glEnd();

	m_batch.draw(m_vertexBuf, m_colorBuf);
}

WorldRenderer::Batch::Batch()
{
	m_numPoints = 0;
	m_vertexArrayID = 0;
	m_vertexBufID = 0;
	m_colorBufID = 0;
}

WorldRenderer::Batch::~Batch()
{
	if (m_vertexArrayID) glDeleteVertexArrays(1, &m_vertexArrayID);
	if (m_vertexBufID) glDeleteBuffers(1, &m_vertexBufID);
	if (m_colorBufID) glDeleteBuffers(1, &m_colorBufID);
}

void WorldRenderer::Batch::init(GLuint numPoints)
{
	m_numPoints = numPoints;

	glGenBuffers(1, &m_vertexBufID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector3f) * numPoints, NULL, GL_DYNAMIC_DRAW);
	glGenBuffers(1, &m_colorBufID);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(M3DVector4f) * numPoints, NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);
	{
		glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufID);
		glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(GLT_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBufID);
		glVertexAttribPointer(GLT_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}
	glBindVertexArray(0);
}

void WorldRenderer::Batch::draw(M3DVector3f* vertexBuf, M3DVector4f* colorBuf)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector3f) * m_numPoints, vertexBuf);

	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M3DVector4f) * m_numPoints, colorBuf);

	glBindVertexArray(m_vertexArrayID);
	glDrawArrays(GL_POINTS, 0, m_numPoints);
	glBindVertexArray(0);
}
