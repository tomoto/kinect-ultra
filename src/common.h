#ifndef _COMMON_H_
#define _COMMON_H_

#pragma warning(disable:4819) // disable annoying warning for UTF-8 characters

// enable memory leak test for Win32 debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// OpenNI
#include <XnCppWrapper.h>
using namespace xn;

#include <GLTools.h> // GLTools

#include <GL/glut.h> // glut

#include <opencv/cv.h> // OpenCV

// Note: I'm using macro instead of inline for some part
// so that I can get enough speed in debug version.
#define USE_MACRO

void registerErrorExitFunc(void(*f)());
void errorExit();

#define CHECK_ERROR(expr, message) check_error(expr, message, #expr);

inline void check_error(int expr, const char* message, const char* detail)
{
	if (!expr) {
		printf("Failed: %s (%s)\n", message, detail);
		errorExit();
	}
}

#define CALL_XN(f) check_xn_status(f, #f);

inline void check_xn_status(XnStatus rc, const char* detail)
{
	if (rc != XN_STATUS_OK) {
		printf("Failed: %s (%s)\n", xnGetStatusString(rc), detail);
		errorExit();
	}
}

#define CALL_GLEW(f) check_glew_status(f, #f)

inline void check_glew_status(XnStatus rc, const char* detail)
{
	if (rc != GLEW_OK) {
		printf("Failed: %s (%s)\n", glewGetErrorString(rc), detail);
		errorExit();
	}
}

#endif
