#ifndef _UTIL_H_
#define _UTIL_H_

#include "common.h"

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

#endif
