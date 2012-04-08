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

#ifndef _COMMON_H_
#define _COMMON_H_

#pragma warning(disable:4819) // disable annoying warning for UTF-8 characters

#define _CRT_SECURE_NO_WARNINGS // disable warnings to fopen, sprintf, etc.

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

// Uncomment this line to output verbose log information.
// TODO: better log facility
// #define ENABLE_LOG

#ifdef ENABLE_LOG
#define LOG(f) (puts("LOG: " #f), (f))
#else
#define LOG(f) f
#endif

#endif
