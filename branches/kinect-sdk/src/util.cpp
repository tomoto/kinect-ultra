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

#include "util.h"
#include "config.h"
#include <opencv/highgui.h>

std::string getResourceFile(const char* category, const char* name)
{
	std::string result;
	result.append("data/").append(category).append("/").append(name);
	return result;
}

static void loadMipmappedTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (!GLEW_VERSION_3_0) {
		LOG( glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE) );
	}
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	if (GLEW_VERSION_3_0) {
		LOG( glGenerateMipmap(GL_TEXTURE_2D) );
	}
}

GLuint readAlphaTexture(const char* file)
{
	// Specify 0 to read the file as grayscale image
	IplImage* image = cvLoadImage(getResourceFile("image", file).c_str(), 0);
	CHECK_ERROR(image != NULL, "The texture image cannot be loaded.");

	GLbyte* pixels = new GLbyte[image->width * image->height * 4];
	GLbyte* p = pixels;
	for (int i = 0; i < image->width * image->height; i++) {
		*p++ = (GLbyte) 0xff;
		*p++ = (GLbyte) 0xff;
		*p++ = (GLbyte) 0xff;
		*p++ = (GLbyte) image->imageData[i];
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	loadMipmappedTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
	cvReleaseImage(&image);

	return textureID;
}

float getPointSize()
{
	return std::max(
		ceil(float(glutGet(GLUT_WINDOW_WIDTH)) / X_RES),
		ceil(float(glutGet(GLUT_WINDOW_HEIGHT)) / Y_RES)
		);
}

void readBatchDef(const char* file, const XnVector3D& origin, float scale, std::vector<GLBatch>* pBatches)
{
	FILE* fp = fopen(file, "r");
	CHECK_ERROR(fp != NULL, "Model definition cannot be opened.");

	GLuint numFaces;
	fscanf(fp, "%d", &numFaces);
	pBatches->resize(numFaces);

	for (std::vector<GLBatch>::iterator b = pBatches->begin(); b != pBatches->end(); b++) {
		GLuint type;
		GLuint numVertices;
		fscanf(fp, "%d %d", &type, &numVertices);
		b->Begin(type, numVertices, 0);
		for (GLuint j = 0; j < numVertices; j++) {
			float x, y, z, nx, ny, nz;
			fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
			b->Normal3f(nx, ny, nz);
			b->Vertex3f(x * scale + origin.X, y * scale + origin.Y, z * scale + origin.Z);
		}
		b->End();
	}

	fclose(fp);
}

void renderStrokeText(const char* text, const XnVector3D& position, const XnVector3D& scale, float thickness, float color[4])
{
	glUseProgram(0);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(position.X, position.Y, position.Z);
	glScalef(scale.X, scale.Y, scale.Z);
	glLineWidth(getPointSize() * thickness);
	glColor4fv(color);
	for (const char* p = text; *p; p++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	}
	glPopMatrix();
}

void getAspectRatioAdjustment(float targetAspectRatio, float areaAspectRatio, float* pScaleX, float *pScaleY)
{
	if (areaAspectRatio >= targetAspectRatio) {
		*pScaleY = 1;
		*pScaleX = targetAspectRatio / areaAspectRatio;
	} else {
		*pScaleY = areaAspectRatio / targetAspectRatio;
		*pScaleX = 1;
	}
}

void adjustViewport()
{
	float scaleX, scaleY;
	int winWidth = glutGet(GLUT_WINDOW_WIDTH);
	int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
	getAspectRatioAdjustment(XY_ASPECT, float(winWidth) / float(winHeight), &scaleX, &scaleY);
	GLsizei vpWidth = GLsizei(winWidth * scaleX);
	GLsizei vpHeight = GLsizei(winHeight * scaleY);
	glViewport((winWidth-vpWidth)/2, (winHeight-vpHeight)/2, vpWidth, vpHeight);
}

void toggleFullScreenMode()
{
	static int x = -1, y, w, h;
	if (x < 0) {
		x = glutGet(GLUT_WINDOW_X);
		y = glutGet(GLUT_WINDOW_Y);
		w = glutGet(GLUT_WINDOW_WIDTH);
		h = glutGet(GLUT_WINDOW_HEIGHT);
		glutFullScreen();
	} else {
		glutPositionWindow(x, y);
		glutReshapeWindow(w, h);
		x = -1;
	}
}

