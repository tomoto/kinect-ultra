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
// DISCLAIMED. IN NO EVENT SHALL TOMOTO S. WASHIO BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//@COPYRIGHT@//

#ifndef _UTIL_H_
#define _UTIL_H_

#include "common.h"
#include "config.h"

const float CONFIDENCE_THRESHOLD = 0.5f;
#ifndef USE_MACRO
inline bool isConfident(XnSkeletonJointPosition jointPos) {
	return jointPos.fConfidence >= CONFIDENCE_THRESHOLD;
}

inline bool isConfident(XnSkeletonJointOrientation jointOrientation)
{
	return jointOrientation.fConfidence >= CONFIDENCE_THRESHOLD;
}
#else
#define isConfident(jointPos) ((jointPos).fConfidence >= CONFIDENCE_THRESHOLD)
#endif

#ifndef USE_MACRO
inline bool i2b(int i) { return !!i; }
#else
#define i2b(i) (!!(i))
#endif

#ifndef USE_MACRO
inline float b2fNormalized(unsigned char b) { return b/255.0f; }
#else
#define b2fNormalized(b) ((b)/255.0f)
#endif

std::string getResourceFile(const char* category, const char* name);

//
// OpenGL utility
//

/**
 * @ja
 * グレースケールの画像を読み込んでアルファテクスチャを作る
 */
GLuint readAlphaTexture(const char* file);

/**
 * @ja
 * 頂点の一覧をファイルから読み込んでGLBatchを作る
 */
void readBatchDef(const char* file, const XnVector3D& origin, float scale, std::vector<GLBatch>* pBatches);

/**
 * @ja
 * Kinectのdepthの値を遠近法で透視した後のZ値に変換する。
 */
#ifndef USE_MACRO
inline float getNormalizedDepth(float z, float nearZ, float farZ)
{
	return (farZ + nearZ) / (farZ - nearZ) / 2.0f - (farZ * nearZ) / (farZ - nearZ) / z + 0.5f;
}
#else
#define getNormalizedDepth(z, nearZ, farZ) (((farZ) + (nearZ)) / ((farZ) - (nearZ)) / 2.0f - ((farZ) * (nearZ)) / ((farZ) - (nearZ)) / (z) + 0.5f)
#endif

#ifndef USE_MACRO
inline float convertRGBtoGray(float r, float g, float b)
{
	return r * 0.3f + g * 0.59f + b * 0.11f;
}
#else
#define convertRGBtoGray(r, g, b) ((r) * 0.3f + (g) * 0.59f + (b) * 0.11f)
#endif

float getPointSize();

void renderStrokeText(const char* text, const XnVector3D& position, const XnVector3D& scale, float thickness, float color[4]);

void getAspectRatioAdjustment(float targetAspectRatio, float areaAspectRatio, float* pScaleX, float *pScaleY);

void adjustViewport();

void toggleFullScreenMode();

#endif
