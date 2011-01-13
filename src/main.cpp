#include "common.h"
#include "config.h"
#include "util.h"

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

#include <GLShaderManager.h>

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
	}
}

static void onGlutDisplay()
{
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

static void checkOpenGLCapabilities()
{
	const char* openGLVersion = (const char*)(glGetString(GL_VERSION));
	printf("OpenGL version = %s\n", openGLVersion);
	const char* shaderLanguageVersion = (const char*)(glGetString(GL_SHADING_LANGUAGE_VERSION));
	if (shaderLanguageVersion) {
		printf("Shader language version = %s\n", shaderLanguageVersion);
	} else {
		puts("Failed: This GPU does not support shaders. Sorry this program cannot run.");
		errorExit();
	}
}

static void initGL(int* pArgc, char* argv[])
{
	glutInit(pArgc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(X_RES, Y_RES);
	glutCreateWindow(WIN_TITLE);
	glutHideWindow();

	checkOpenGLCapabilities();

	glutKeyboardFunc(onGlutKeyboard);
	glutDisplayFunc(onGlutDisplay);
	glutIdleFunc(onGlutIdle);

	CALL_GLEW( glewInit() );
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

	initProjection();

	g_voxelObjectMapper = new VoxelObjectMapper(&g_depthGen);
	g_sparkRenderer = new SparkRenderer(&g_renderingCtx);

	g_flatImageRenderer = new ImageRenderer(&g_renderingCtx, &g_imageGen);
	g_flatImageRenderer->lock(false);
	g_worldRenderer = new WorldRenderer(&g_renderingCtx, &g_depthGen, &g_imageGen, g_henshinDetector);
	g_eyeSluggerRenderer = new EyeSluggerRenderer(&g_renderingCtx, g_henshinDetector);
	g_eyeSluggerDetector = new EyeSluggerDetector(g_henshinDetector, g_eyeSluggerRenderer);
	g_skeletonRenderer = new SkeletonRenderer(&g_renderingCtx, &g_depthGen, g_userDetector, g_henshinDetector);
	g_wideshotRenderer = new WideshotRenderer(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer);
	g_wideshotDetector = new WideshotDetector(&g_depthGen, g_userDetector, g_wideshotRenderer);
	g_emeriumBeamRenderer1 = new EmeriumBeamRenderer1(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer);
	g_emeriumBeamRenderer2 = new EmeriumBeamRenderer2(&g_renderingCtx, g_voxelObjectMapper, g_sparkRenderer);
	g_emeriumBeamDetector = new EmeriumBeamDetector1(&g_depthGen, g_userDetector, g_emeriumBeamRenderer1);
	g_emeriumBeamDetector2 = new EmeriumBeamDetector2(&g_depthGen, g_userDetector, g_emeriumBeamRenderer2);

	// g_testTorusRenderer = new TestTorusRenderer(&g_renderingCtx);

	takeImageSnapshot();
}

static void displayWelcomeMessage()
{
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
		puts("Welcome to Ultra Seven on Kinect!");
		puts("");
		puts("Available keys during the play:");
		puts("[ESC]  -- Exit");
		puts("[q][a] -- Adjust the depth of 3D virtual objects.");
		puts("[s]    -- Retake the background image overlayed when you fly.");
		puts("");
		puts("It may take a minute until initialization completes... Enjoy!");
		puts("");
	}
}

void main(int argc, char* argv[])
{
	displayWelcomeMessage();

	initGL(&argc, argv);
	initXN();
	initRenderers();
	glutShowWindow();
	glutMainLoop();
}
