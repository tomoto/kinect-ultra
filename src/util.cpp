#include "util.h"
#include "config.h"
#include <opencv/highgui.h>

std::string getResourceFile(const char* category, const char* name)
{
	std::string result;
	result.append("data/").append(category).append("/").append(name);
	return result;
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

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

