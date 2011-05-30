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

#include "common.h"
#include "config.h"
#include "util.h"

#include "Configuration.h"

#include "RenderingContext.h"

#include "ImageRenderer.h"
#include "WorldRenderer.h"
#include "SkeletonRenderer.h"
#include "UserDetector.h"
#include "HenshinDetector.h"
#include "WideshotDetector.h"
#include "EmeriumBeamDetector1.h"
#include "EmeriumBeamDetector2.h"
#include "EmeriumBeamRenderer1.h"
#include "EmeriumBeamRenderer2.h"
#include "EyeSluggerDetector.h"
#include "EyeSluggerRenderer.h"
#include "VoxelObjectMapper.h"
#include "TestTorusRenderer.h"
#include "FrameRateCounter.h"

#include <GLShaderManager.h>

#define APP_VERSION "0.1f-rc"

// OpenNI objects
Context g_context;
DepthGenerator g_depthGen;
ImageGenerator g_imageGen;
UserGenerator g_userGen;

// GL objects
GLShaderManager g_shaderMan;

// App objects
RenderingContext g_renderingCtx;

ImageRenderer* g_flatImageRenderer;
WorldRenderer* g_worldRenderer;
SkeletonRenderer* g_skeletonRenderer;
WideshotRenderer* g_wideshotRenderer;
EmeriumBeamRenderer1* g_emeriumBeamRenderer1;
EmeriumBeamRenderer2* g_emeriumBeamRenderer2;
SparkRenderer* g_sparkRenderer;
EyeSluggerRenderer* g_eyeSluggerRenderer;
//TestTorusRenderer* g_testTorusRenderer;

UserDetector* g_userDetector;
HenshinDetector* g_henshinDetector;
WideshotDetector* g_wideshotDetector;
EmeriumBeamDetector1* g_emeriumBeamDetector;
EmeriumBeamDetector2* g_emeriumBeamDetector2;
VoxelObjectMapper* g_voxelObjectMapper;
EyeSluggerDetector* g_eyeSluggerDetector;

FrameRateCounter g_frameRateCounter;

static void takeImageSnapshot()
{
	g_flatImageRenderer->lock(false);
	g_flatImageRenderer->draw();
	g_flatImageRenderer->lock(true);
}

static void onGlutKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(1);
		case 'q':
			g_worldRenderer->addDepthAdjustment(5);
			break;
		case 'a':
			g_worldRenderer->addDepthAdjustment(-5);
			break;
		case 's':
			takeImageSnapshot();
			break;
		case 'f':
			g_frameRateCounter.toggleEnabled();
			break;
		case 't':
			Configuration::getInstance()->changeTriggerHappyMode();
			break;
		case 'm':
			g_renderingCtx.mirror();
			break;
	}
}

static void onGlutDisplay()
{
	g_frameRateCounter.update();

	g_context.WaitAndUpdateAll();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	g_voxelObjectMapper->update();
	g_henshinDetector->detect();
	g_wideshotDetector->detect();
	g_emeriumBeamDetector->detect();
	g_emeriumBeamDetector2->detect();
	g_eyeSluggerDetector->detect();

	if (g_henshinDetector->getStage() == HenshinDetector::STAGE_DEHENSHINING) {
		g_flatImageRenderer->draw();
	}
	g_worldRenderer->draw();
	g_eyeSluggerRenderer->draw();
	g_skeletonRenderer->draw();
	g_wideshotRenderer->draw();
	g_emeriumBeamRenderer1->draw();
	g_emeriumBeamRenderer2->draw();
	g_sparkRenderer->draw();

	// g_testTorusRenderer->draw();
	glutSwapBuffers();
}
static void onGlutIdle()
{
	glutPostRedisplay();
}

static void initXN()
{
	CALL_XN( g_context.InitFromXmlFile(getResourceFile("config", "OpenNIConfig.xml").c_str()) );
	CALL_XN( g_context.SetGlobalMirror(TRUE) );
	CALL_XN( g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_imageGen) );
	CALL_XN( g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depthGen) );
	CALL_XN( g_depthGen.GetAlternativeViewPointCap().SetViewPoint(g_imageGen) );
	CALL_XN( g_context.FindExistingNode(XN_NODE_TYPE_USER, g_userGen) );
	CALL_XN( g_userGen.Create(g_context) );
	g_userGen.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	ImageMetaData imageMD;
	g_imageGen.GetMetaData(imageMD);
	CHECK_ERROR(imageMD.PixelFormat() == XN_PIXEL_FORMAT_RGB24, "This camera is not supported.");

	g_userDetector = new UserDetector(&g_userGen);
	g_henshinDetector = new HenshinDetector(g_userDetector);

	CALL_XN( g_context.StartGeneratingAll() );
}

static void sorryThisProgramCannotRunBecause(const char* reason)
{
	printf("Failed: Sorry this program cannot run because %s.\n", reason);
	errorExit();
}

static void checkOpenGLCapabilities()
{
	const char* openGLVersion = (const char*)(glGetString(GL_VERSION));
	printf("OpenGL version = %s\n", openGLVersion);

	const char* shaderLanguageVersion = (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION));
	if (shaderLanguageVersion) {
		printf("Shader language version = %s\n", shaderLanguageVersion);
	} else {
		sorryThisProgramCannotRunBecause("this GPU does not support programmable shaders");
	}

	if (glGenVertexArrays) {
		// VAOs are supported. Good.
	} else {
		sorryThisProgramCannotRunBecause("VAOs are not supported");
	}
}

static void initGL(int* pArgc, char* argv[])
{
	glutInit(pArgc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(X_RES, Y_RES);
	glutCreateWindow(WIN_TITLE);
	glutHideWindow();
	CALL_GLEW( glewInit() );

	checkOpenGLCapabilities();

	glutKeyboardFunc(onGlutKeyboard);
	glutDisplayFunc(onGlutDisplay);
	glutIdleFunc(onGlutIdle);

	g_shaderMan.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);

	// enable smoothing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	// glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
}

static void initProjection()
{
	XnFieldOfView fov;
	g_depthGen.GetFieldOfView(fov);

	GLFrustum frustum;
	float verticalFOVInDegree = float(m3dRadToDeg(fov.fVFOV));
	float aspect = float(tan(fov.fHFOV/2)/tan(fov.fVFOV/2));
	frustum.SetPerspective(verticalFOVInDegree, aspect, PERSPECTIVE_Z_MIN, PERSPECTIVE_Z_MAX);

	g_renderingCtx.projectionMatrix.LoadMatrix(frustum.GetProjectionMatrix());
	g_renderingCtx.projectionMatrix.Scale(-1, 1, -1);
}

static void initRenderers()
{
	g_renderingCtx.shaderMan = &g_shaderMan;

	LOG( initProjection() );

	LOG( g_voxelObjectMapper = new VoxelObjectMapper(&g_depthGen) );
	LOG( g_sparkRenderer = new SparkRenderer(&g_renderingCtx) );

	LOG( g_flatImageRenderer = new ImageRenderer(&g_renderingCtx, &g_imageGen) );
	LOG( g_flatImageRenderer->lock(false) );
	LOG( g_worldRenderer = new WorldRenderer(&g_renderingCtx, &g_depthGen, &g_imageGen, g_henshinDetector) );
	LOG( g_eyeSluggerRenderer = new EyeSluggerRenderer(&g_renderingCtx, g_henshinDetector) );
	LOG( g_eyeSluggerDetector = new EyeSluggerDetector(g_henshinDetector, g_eyeSluggerRenderer) );
	LOG( g_skeletonRenderer = new SkeletonRenderer(&g_renderingCtx, &g_depthGen, g_userDetector, g_henshinDetector) );
	LOG( g_wideshotRenderer = new WideshotRenderer(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer) );
	LOG( g_wideshotDetector = new WideshotDetector(&g_depthGen, g_userDetector, g_wideshotRenderer) );
	LOG( g_emeriumBeamRenderer1 = new EmeriumBeamRenderer1(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer) );
	LOG( g_emeriumBeamRenderer2 = new EmeriumBeamRenderer2(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer) );
	LOG( g_emeriumBeamDetector = new EmeriumBeamDetector1(&g_depthGen, g_userDetector, g_emeriumBeamRenderer1) );
	LOG( g_emeriumBeamDetector2 = new EmeriumBeamDetector2(&g_depthGen, g_userDetector, g_emeriumBeamRenderer2) );

	// g_testTorusRenderer = new TestTorusRenderer(&g_renderingCtx);

	LOG( takeImageSnapshot() );
}

static void displayWelcomeMessage()
{
	puts("kinect-ultra " APP_VERSION);
	char file[64];
	char line[256];
	sprintf(file, "welcome-%d.txt", GetConsoleCP());
	FILE* fp = fopen(getResourceFile("message", file).c_str(), "r");
	if (fp) {
		while (fgets(line, sizeof(line) - 1, fp)) {
			fputs(line, stdout);
		}
		fclose(fp);
	} else {
		//    123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
		puts("Welcome to Ultra Seven on Kinect!");
		puts("");
		puts("Available keys during the play:");
		puts("[ESC]  -- Exit");
		puts("[q][a] -- Adjust the depth of 3D virtual objects.");
		puts("[s]    -- Retake the background image overlayed when you fly.");
		puts("[f]    -- Output framerate to the console.");
		puts("[m]    -- Mirror the screen.");
		puts("[t]    -- Change Trigger Happy Mode. (Normal/Happy/Happier)");
		puts("          This makes it easier to trigger the powers.");
		puts("");
		puts("It may take a minute until initialization completes... Enjoy!");
		puts("");
	}
}

void main(int argc, char* argv[])
{
	// enable memory leak report for Win32 debug
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	displayWelcomeMessage();

	initGL(&argc, argv);
	initXN();
	initRenderers();
	glutShowWindow();
	glutMainLoop();
}
