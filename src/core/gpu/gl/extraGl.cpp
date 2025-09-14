#include "extraGl.h"

#ifdef __EMSCRIPTEN__

bool extraGlInit()
{
	return true;
}

#else

PFNGLGETTEXIMAGEPROC glGetTexImage;
PFNGLPIXELSTOREIPROC glPixelStorei;
PFNGLREADBUFFERPROC glReadBuffer;
PFNGLREADPIXELSPROC glReadPixels;

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)

static HMODULE _libGL = NULL;

typedef PROC(WINAPI* PFNGLGETPROCADDRESSPROC)(LPCSTR);
static PFNGLGETPROCADDRESSPROC glGetProcAddress = NULL;

static bool _glLoad()
{
	_libGL = LoadLibraryW(L"opengl32.dll");
	if (!_libGL)
	{
		return false;
	}
	if (!glGetProcAddress)
		glGetProcAddress = (PFNGLGETPROCADDRESSPROC) GetProcAddress(_libGL, "wglGetProcAddress");
	if (!glGetProcAddress)
		glGetProcAddress = (PFNGLGETPROCADDRESSPROC) GetProcAddress(_libGL, "wglGetProcAddressARB");
	if (!glGetProcAddress)
		return false;

	if (!_libGL)
	{
		return false;
	}
	return true;
}

// load opengl proc address from dll or from wglGetProcAddress
static PROC _getProcAddress(const char* procName)
{
	auto procHandle = GetProcAddress(_libGL, procName);

	if (!procHandle)
		procHandle = glGetProcAddress(procName);
	return procHandle;
}
// #endif

// #ifdef defined(__APPLE__) || defined(__MACH__)
#else
#include <dlfcn.h>
static void* _libGL = nullptr;

static bool _glLoad()
{
	if (!_libGL)
		_libGL = dlopen("/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
	if (!_libGL)
		_libGL = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
	if (_libGL)
		return true;
	return false;
}

static void* _getProcAddress(const char* procName)
{
	return dlsym(_libGL, procName);
}
#endif

#define GL_FUNCTION_FETCH(procName, procType)         \
	procName = (procType) _getProcAddress(#procName); \
	if (!procName)                                    \
	{                                                 \
		return false;                                 \
	}

bool extraGlInit()
{
	if (!_glLoad())
		return false;

	GL_FUNCTION_FETCH(glGetTexImage, PFNGLGETTEXIMAGEPROC);
	GL_FUNCTION_FETCH(glPixelStorei, PFNGLPIXELSTOREIPROC);
	GL_FUNCTION_FETCH(glReadBuffer, PFNGLREADBUFFERPROC);
	GL_FUNCTION_FETCH(glReadPixels, PFNGLREADPIXELSPROC);
	return true;
}

#endif
