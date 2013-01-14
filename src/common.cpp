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

static void (*s_errorExitFunc)();

void errorExit()
{
	printf("Error occured. Press [ENTER] to close this window.\n");
	if (s_errorExitFunc) {
		s_errorExitFunc();
	}
	getchar();
	exit(1);
}

void registerErrorExitFunc(void(*f)())
{
	s_errorExitFunc = f;
}

void checkError(int expr, const char* message, const char* detail)
{
	if (!expr) {
		printf("Failed: %s (%s)\n", message, detail);
		errorExit();
	}
}

#ifdef XU_KINECTSDK
#include "nui_error.h"

static int getHResultString(HRESULT hr, char* buf, int size)
{
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, size, NULL);
	return strlen(chomp(buf));
}

void checkWin32Status(HRESULT hr, const char* detail)
{
	if (FAILED(hr)) {
		char msg[1024];
		getHResultString(hr, msg, sizeof(msg)) || strcpy(msg, "?");
		printf("Failed: %s [%08x] (%s)\n", msg, hr, detail);
		errorExit();
	}
}

void checkNuiStatus(HRESULT hr, const char* detail)
{
	if (FAILED(hr)) {
		char msg[1024];
		getNuiErrorString(hr, msg, sizeof(msg)) || strcpy(msg, "?");
		printf("Failed: %s [%08x] (%s)\n", msg, hr, detail);
		errorExit();
	}
}

#elif defined XU_OPENNI2
void checkXnStatus(openni::Status rc, const char* detail)
{
	if (rc != openni::STATUS_OK) {
		printf("OpenNI Failed: %s (%s)\n", openni::OpenNI::getExtendedError(), detail);
		errorExit();
	}
}

void checkXnStatus(nite::Status rc, const char* detail)
{
	if (rc != nite::STATUS_OK) {
		printf("NiTE Failed: %d (%s)\n", rc, detail);
		errorExit();
	}
}

#else // XU_OPENNI
void checkXnStatus(XnStatus rc, const char* detail)
{
	if (rc != XN_STATUS_OK) {
		printf("Failed: %s (%s)\n", xnGetStatusString(rc), detail);
		errorExit();
	}
}
#endif

void checkGlewStatus(int rc, const char* detail)
{
	if (rc != GLEW_OK) {
		printf("Failed: %s (%s)\n", glewGetErrorString(rc), detail);
		errorExit();
	}
}
