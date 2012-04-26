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
#include <assert.h>

#define NOMINMAX
#include <Windows.h>

#ifdef XU_KINECTSDK
#include <NuiApi.h>
#else // XU_OPENNI
#include <XnCppWrapper.h>
using namespace xn;
#endif

#include <GLTools.h> // GLTools

#include <GL/glut.h> // glut

// types
#ifdef XU_KINECTSDK
typedef USHORT XuRawUserIDPixel;
typedef DWORD XuUserID;
struct XuColorPixel { BYTE nBlue; BYTE nGreen; BYTE nRed; BYTE nAlpha; };
struct XuRawColorPixel { BYTE nBlue; BYTE nGreen; BYTE nRed; BYTE nAlpha; };
const GLenum XU_RAW_COLOR_PIXEL_GL_FORMAT = GL_BGRA;
typedef DWORD XuDepthPixel;
typedef USHORT XuRawDepthPixel;
#else // XU_OPENNI
typedef XnLabel XuRawUserIDPixel;
typedef XnUserID XuUserID;
typedef XnRGB24Pixel XuColorPixel;
typedef XnRGB24Pixel XuRawColorPixel;
const GLenum XU_RAW_COLOR_PIXEL_GL_FORMAT = GL_RGB;
typedef XnDepthPixel XuDepthPixel;
typedef XnDepthPixel XuRawDepthPixel;
#endif

// Note: I'm using macro instead of inline for some part
// so that I can get enough speed in debug version.
#define USE_MACRO

#ifdef XU_KINECTSDK
#ifndef USE_MACRO
inline GetDepthFromRawPixel(XuRawDepthPixel d) { return NuiDepthPixelToDepth(d); }
inline GetUserIDFromRawPixel(XuRawUserIDPixel u) { return NuiDepthPixelToPlayerIndex(u); }
#else
#define GetDepthFromRawPixel(d) NuiDepthPixelToDepth(d)
#define GetUserIDFromRawPixel(u) NuiDepthPixelToPlayerIndex(u)
#endif
#else // XU_OPENNI
#define GetDepthFromRawPixel(d) (d)
#define GetUserIDFromRawPixel(u) (u)
#endif

void registerErrorExitFunc(void(*f)());
void errorExit();

#define CHECK_ERROR(expr, message) checkError(expr, message, #expr);
void checkError(int expr, const char* message, const char* detail);

#define CALL_WIN32 checkWin32Status(f, #f);
void checkWin32Status(HRESULT hr, const char* detail);

#ifdef XU_KINECTSDK
#define CALL_SENSOR(f) checkNuiStatus(f, #f);
void checkNuiStatus(HRESULT hr, const char* detail);
#else // XU_OPENNI
#define CALL_SENSOR(f) checkXnStatus(f, #f);
void checkXnStatus(XnStatus rc, const char* detail);
#endif

#define CALL_GLEW(f) checkGlewStatus(f, #f)
void checkGlewStatus(int rc, const char* detail);

// Uncomment this line to output verbose log information.
// TODO: better log facility
// #define ENABLE_LOG

#ifdef ENABLE_LOG
#define LOG(f) (puts("LOG: " #f), (f))
#else
#define LOG(f) f
#endif

#endif
