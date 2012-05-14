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
#include "util.h"
#include "Holder.h"

#include "Configuration.h"

#include "RenderingContext.h"

#include "SensorManager.h"
#include "ImageProvider.h"
#include "DepthProvider.h"
#include "UserProvider.h"

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
#include "EyeSluggerDetectorEx.h"
#include "EyeSluggerRenderer.h"
#include "UltraEyeRenderer.h"
#include "VoxelObjectMapper.h"
//#include "TestTorusRenderer.h"
#include "FrameRateCounter.h"

#include <GLShaderManager.h>

#ifdef XU_KINECTSDK
#define APP_VERSION_FOR "Kinect SDK"
#else // XU_OPENNI
#define APP_VERSION_FOR "OpenNI"
#endif

#define APP_VERSION_NUMBER "1.0b"
#define APP_VERSION APP_VERSION_NUMBER " (" APP_VERSION_FOR ")"

// Sensor objects
static Holder<SensorManager> s_sensorMan;

// GL objects
static GLShaderManager s_shaderMan;

// App objects
static RenderingContext* s_renderingContext;

static ImageRenderer* s_flatImageRenderer;
static WorldRenderer* s_worldRenderer;
static SkeletonRenderer* s_skeletonRenderer;
static WideshotRenderer* s_wideshotRenderer;
static EmeriumBeamRenderer1* s_emeriumBeamRenderer1;
static EmeriumBeamRenderer2* s_emeriumBeamRenderer2;
static SparkRenderer* s_sparkRenderer;
static EyeSluggerRendererEx* s_eyeSluggerRenderer;
static UltraEyeRenderer* s_ultraEyeRenderer;
//TestTorusRenderer* s_testTorusRenderer;

static UserDetector* s_userDetector;
static HenshinDetector* s_henshinDetector;
static WideshotDetector* s_wideshotDetector;
static EmeriumBeamDetector1* s_emeriumBeamDetector1;
static EmeriumBeamDetector2* s_emeriumBeamDetector2;
static VoxelObjectMapper* s_voxelObjectMapper;
static EyeSluggerDetectorEx* s_eyeSluggerDetector;

static FrameRateCounter s_frameRateCounter;

static void takeImageSnapshot()
{
	s_flatImageRenderer->lock(false);
}

static void onGlutKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(1);
		case 13:
			toggleFullScreenMode();
		case 'q':
			s_worldRenderer->addDepthAdjustment(5);
			break;
		case 'a':
			s_worldRenderer->addDepthAdjustment(-5);
			break;
		case 's':
			takeImageSnapshot();
			break;
		case 'f':
			s_frameRateCounter.toggleEnabled();
			break;
		case 't':
			Configuration::getInstance()->changeTriggerHappyMode();
			break;
		case 'm':
			s_renderingContext->mirror();
			break;
	}
}

static void onGlutDisplay()
{
	if (!s_sensorMan->waitAllForNextFrameAndLock()) return;

	s_frameRateCounter.update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	adjustViewport();

	s_voxelObjectMapper->update();
	s_userDetector->detect();
	s_henshinDetector->detect();
	s_wideshotDetector->detect();
	s_emeriumBeamDetector1->detect();
	s_emeriumBeamDetector2->detect();
	s_eyeSluggerDetector->detect();

	if (!s_flatImageRenderer->isLocked() ||
			s_henshinDetector->getStage() == HenshinDetector::STAGE_DEHENSHINING) {
		s_flatImageRenderer->draw();
		s_flatImageRenderer->lock(true);
	}

	s_worldRenderer->draw();
	s_skeletonRenderer->draw();
	s_ultraEyeRenderer->draw();
	s_eyeSluggerRenderer->draw();
	s_wideshotRenderer->draw();
	s_emeriumBeamRenderer1->draw();
	s_emeriumBeamRenderer2->draw();
	s_sparkRenderer->draw();

	// s_testTorusRenderer->draw();
	glutSwapBuffers();

	s_sensorMan->unlock();
}

static void onGlutIdle()
{
	glutPostRedisplay();
}

static void initSensor()
{
	s_sensorMan = new SensorManager();
	s_userDetector = new UserDetector(s_sensorMan->getUserProvider());
	s_henshinDetector = new HenshinDetector(s_userDetector);
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

	s_shaderMan.InitializeStockShaders();

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
	float hfov, vfov;
	s_sensorMan->getDepthProvider()->getFOV(&hfov, &vfov);

	GLFrustum frustum;
	float verticalFOVInDegree = float(m3dRadToDeg(vfov));
	float aspect = float(tan(hfov/2)/tan(vfov/2));
	frustum.SetPerspective(verticalFOVInDegree, aspect, PERSPECTIVE_Z_MIN, PERSPECTIVE_Z_MAX);

	s_renderingContext->projectionMatrix.LoadMatrix(frustum.GetProjectionMatrix());
	s_renderingContext->projectionMatrix.Scale(-1, 1, -1);
}

static void initRenderers()
{
	s_renderingContext = new RenderingContext(&s_shaderMan);

	LOG( initProjection() );

	DepthProvider* depthProvider = s_sensorMan->getDepthProvider();
	ImageProvider* imageProvider = s_sensorMan->getImageProvider();

	LOG( s_voxelObjectMapper = new VoxelObjectMapper(depthProvider) );
	LOG( s_sparkRenderer = new SparkRenderer(s_renderingContext) );

	LOG( s_flatImageRenderer = new ImageRenderer(s_renderingContext, imageProvider) );
	LOG( s_flatImageRenderer->lock(false) );
	LOG( s_worldRenderer = new WorldRenderer(s_renderingContext, depthProvider, imageProvider, s_henshinDetector) );
	LOG( s_skeletonRenderer = new SkeletonRenderer(s_renderingContext, depthProvider, s_henshinDetector) );
	LOG( s_eyeSluggerRenderer = new EyeSluggerRendererEx(s_renderingContext, s_henshinDetector) );
	LOG( s_eyeSluggerDetector = new EyeSluggerDetectorEx(s_henshinDetector, s_eyeSluggerRenderer) );
	LOG( s_wideshotRenderer = new WideshotRenderer(s_renderingContext, s_voxelObjectMapper, s_sparkRenderer) );
	LOG( s_wideshotDetector = new WideshotDetector(depthProvider, s_henshinDetector, s_wideshotRenderer) );
	LOG( s_emeriumBeamRenderer1 = new EmeriumBeamRenderer1(s_renderingContext, s_voxelObjectMapper, s_sparkRenderer) );
	LOG( s_emeriumBeamRenderer2 = new EmeriumBeamRenderer2(s_renderingContext, s_voxelObjectMapper, s_sparkRenderer) );
	LOG( s_emeriumBeamDetector1 = new EmeriumBeamDetector1(depthProvider, s_henshinDetector, s_emeriumBeamRenderer1) );
	LOG( s_emeriumBeamDetector2 = new EmeriumBeamDetector2(depthProvider, s_henshinDetector, s_emeriumBeamRenderer2) );
	LOG( s_ultraEyeRenderer = new UltraEyeRenderer(s_renderingContext, s_henshinDetector) );

	 //s_testTorusRenderer = new TestTorusRenderer(&s_renderingContext);

//	// LOG( s_renderingContext.mirror() ); // remove comment to flip the screen by default
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
		puts("Welcome to kinect-ultra!");
		puts("");
		puts("Available keys during the play:");
		puts("[ESC]  -- Exit");
		puts("[ENTER]-- Toggle full screen");
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
	initSensor();
	initRenderers();
	glutShowWindow();
	// toggleFullScreenMode(); // remove comment to run in the full-screen mode by default
	glutMainLoop();
}
