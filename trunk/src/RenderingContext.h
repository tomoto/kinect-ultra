#ifndef _RENDERING_CONTEXT_
#define _RENDERING_CONTEXT_

#include "common.h"
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

class RenderingContext
{
public:
	GLShaderManager* shaderMan;

	GLMatrixStack modelViewMatrix;
	GLMatrixStack projectionMatrix;
	GLFrame cameraFrame;
	GLGeometryTransform transform;

public:
	RenderingContext()
	{
		transform.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	}

	~RenderingContext()
	{
	}

	void cameraFrameUpdated()
	{
		M3DMatrix44f cm;
		cameraFrame.GetCameraMatrix(cm);
		modelViewMatrix.LoadMatrix(cm);
	}
};

#endif
