/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : boot.cpp
// Description : Windows bootstrap
//============================================================================

#ifdef _WIN32

#include "../../Logger.h"
#include "resources.h"
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include "../platformgl.h"
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
#include <conio.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "../../batterytech_globals.h"
#include "../../Context.h"

using namespace std;
using namespace BatteryTech;

#ifdef __MINGW32__
#define _LPCSTR LPCSTR
#else
#define _LPCSTR LPCWSTR
#endif

// OpenGL 1.3 extension function pointers
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = NULL;

// OpenGL 1.5 extension function pointers
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLISBUFFERPROC glIsBuffer = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;

// OpenGL 2.0 extension function pointers (almost all but the VertexAttrib "s" and "d" functions are ES 2.0 compatible)
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMPROC glIsProgram = NULL;
PFNGLISSHADERPROC glIsShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM2FPROC glUniform2f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2IPROC glUniform2i = NULL;
PFNGLUNIFORM3IPROC glUniform3i = NULL;
PFNGLUNIFORM4IPROC glUniform4i = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORM1IVPROC glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

// 2.0 framebuffer/renderbuffer extensions
PFNGLISRENDERBUFFERPROC glIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;

// WinMain
DWORD WINAPI StartThread(LPVOID iValue);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
BOOL leftButtonDown = FALSE;
BOOL32 quit = FALSE;
HWND hWnd;

extern "C" {

void setupConsole() {
	// set up console
	int iConsoleHandle;
	long lStdHandle;
	FILE* pFile;
	CONSOLE_SCREEN_BUFFER_INFO CSBIConsoleInfo;
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),  &CSBIConsoleInfo);
	CSBIConsoleInfo.dwSize.Y = 1024;SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), CSBIConsoleInfo.dwSize);
	//Redirect to the console
	lStdHandle = reinterpret_cast<long>(GetStdHandle(STD_OUTPUT_HANDLE));
	iConsoleHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	pFile = _fdopen( iConsoleHandle, "w" );
	*stdout = *pFile;
	setvbuf( stdout, NULL, _IONBF, 0 );
	lStdHandle = reinterpret_cast<long>(GetStdHandle(STD_ERROR_HANDLE));
	iConsoleHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	pFile = _fdopen( iConsoleHandle, "w" );
	*stderr = *pFile;
	setvbuf( stderr, NULL, _IONBF, 0 );
	std::ios::sync_with_stdio();
}

void createWindow(HINSTANCE hInstance, long width, long height, const char *title) {
	WNDCLASS wc;
	DWORD dwExStyle; // Window Extended Style
	DWORD dwStyle; // Window Style
	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = static_cast<HICON> (LoadImage(hInstance,
			MAKEINTRESOURCE(AppIcon), IMAGE_ICON, 32, 32, LR_DEFAULTSIZE));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCSTR)"BTApp";
	RegisterClass(&wc);

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	RECT WindowRect;
	WindowRect.left = (long) 0;
	WindowRect.right = (long) width;
	WindowRect.top = (long) 0;
	WindowRect.bottom = (long) height;
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size
	// create main window
	/*hWnd = CreateWindow(
	 "BatteryTech", "BatteryTech",
	 WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
	 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT,
	 NULL, NULL, hInstance, NULL );*/
	DWORD dwWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	DWORD dwHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	hWnd = CreateWindowEx(dwExStyle, // Extended Style For The Window
			(LPCSTR)"BTApp", // Class Name
			(LPCSTR)title, // Window Title
			WS_CLIPSIBLINGS | // Required Window Style
					WS_CLIPCHILDREN | // Required Window Style
					WS_VISIBLE | dwStyle, // Selected Window Style
					dwWidth / 2 - width / 2, dwHeight /2 - height / 2, // Window Position
			WindowRect.right - WindowRect.left, // Calculate Adjusted Window Width
			WindowRect.bottom - WindowRect.top, // Calculate Adjusted Window Height
			NULL, // No Parent Window
			NULL, // No Menu
			hInstance, // Instance
			NULL);

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int iCmdShow) {
	MSG msg;

	// what modern PC video card doesn't support shaders?
	GraphicsConfiguration *gConfig = new GraphicsConfiguration;
	gConfig->supportsHWmipmapgen = true;
	gConfig->supportsUVTransform = true;
	gConfig->supportsVBOs = true;
	// with windows we assume shader support but will correct it later if this is wrong
	gConfig->supportsShaders = true;
	btInit(gConfig, 0, 0);
	Context *context = btGetContext();
	HANDLE hThread1 = NULL;
	DWORD dwGenericThread;
	S32 windowWidth = 640;
	S32 windowHeight = 640;
	const char *windowName = "BatteryTech";
	BOOL32 console = TRUE;
	if (context->appProperties) {
		windowWidth = context->appProperties->get("window_width")->getIntValue();
		windowHeight = context->appProperties->get("window_height")->getIntValue();
		console = context->appProperties->get("console_log_enabled_when_available")->getBoolValue();
		btSetScreenSize(windowWidth, windowHeight);
		windowName = context->appProperties->get("windowed_app_name")->getValue();
	}
	createWindow(hInstance, windowWidth, windowHeight, windowName);
	if (console) {
		setupConsole();
	}
	if (!context->appProperties) {
		cout << "Something went wrong initializing BatteryTech before the console was opened.  Check batterytech.log for details" << endl;
	} else {
		hThread1 = CreateThread(NULL, 0, StartThread, NULL, 0, &dwGenericThread);
	}
	if (!hThread1) {
		cout << "Error creating thread" << endl;
	} else {
		SetPriorityClass(hInstance, HIGH_PRIORITY_CLASS);
		SetThreadPriority(hThread1, THREAD_PRIORITY_HIGHEST);
	}
	// program main loop
	while (!quit) {
		// check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE )) {
			// handle or dispatch messages
			if (msg.message == WM_QUIT) {
				quit = TRUE;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		::Sleep(16);
	}
	WaitForSingleObject(hThread1, INFINITE);
	// destroy the window explicitly
	DestroyWindow(hWnd);
	delete gConfig;
	gConfig = NULL;
	return msg.wParam;
}

}

DWORD WINAPI StartThread(LPVOID iValue) {
	cout << "Starting Game Thread" << endl;
	HGLRC hRC;
	HDC hDC;
	// enable OpenGL for the window
	cout << "Enabling OpenGL" << endl;
	EnableOpenGL(hWnd, &hDC, &hRC);
	BOOL shadersSupported = (glCreateShader != NULL);
	if (shadersSupported) {
		logmsg("Shaders supported");
		btGetContext()->gConfig->supportsShaders = true;
	} else {
		logmsg("Shaders not supported");
		btGetContext()->gConfig->supportsShaders = false;
		btGetContext()->gConfig->useShaders = FALSE;
	}
	BOOL32 useShaders = btGetContext()->appProperties->get("use_shaders")->getBoolValue();
	if (useShaders && !shadersSupported) {
		MessageBox(NULL, "This BatteryTech application requires a minimum of OpenGL 2.0", NULL, 0);
		quit = true;
	} else {
		DWORD time = timeGetTime();
		DWORD oldTime = time;
		DWORD timeDeltaMs;
		while (!quit) {
			oldTime = time;
			time = timeGetTime();
			timeDeltaMs = time - oldTime;
			btUpdate(timeDeltaMs / 1000.0f);
			//cout << "Updating " << (time - oldTime) / 1000.0f << endl;
			btDraw();
			SwapBuffers(hDC);
			::Sleep(16); // limit to 60fps on PC.
			// A better thing to do would be to use a high res timer and actually calc out the amount of time to wait.
			// However the target is mobile and will always have less CPU so this could give a better indication of FPS drop.
		}
		// stop sound process before releasing app
	}
	btRelease();
	// shutdown OpenGL
	DisableOpenGL(hWnd, hDC, hRC);
	return 0;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	int x,y,z;
	switch (message) {
	case WM_CHAR:
		btKeyPressed(wParam, BatteryTech::SKEY_NULL);
		return 0;
	case WM_KEYUP:
		btKeyUp(wParam, BatteryTech::SKEY_NULL);
		return 0;
	case WM_KEYDOWN:
		btKeyDown(wParam, BatteryTech::SKEY_NULL);
		return 0;
	case WM_LBUTTONDOWN:
		leftButtonDown = TRUE;
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		btSetPointerState(0, true, x, y);
		return 0;
	case WM_LBUTTONUP:
		leftButtonDown = FALSE;
		btSetPointerState(0, false, 0, 0);
		return 0;
	case WM_MOUSEMOVE:
		if (leftButtonDown) {
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);
			btSetPointerState(0, true, x, y);
		}
		return 0;
	case WM_MOUSEWHEEL:
		z = GET_WHEEL_DELTA_WPARAM(wParam);
		if (z > 0) {
			btKeyPressed(0, BatteryTech::SKEY_WHEELUP);
		} else {
			btKeyPressed(0, BatteryTech::SKEY_WHEELDOWN);
		}
	case WM_CREATE:
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_DESTROY:
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

}

// Enable OpenGL

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
typedef BOOL (APIENTRY *PFNGLXSWAPINTERVALFARPROC)( int );
PFNGLXSWAPINTERVALFARPROC glxSwapIntervalSGI = 0;

PROC wglLoadExtension(const char* name) {
	//void* result = NULL;
	PROC result = wglGetProcAddress(name);
   if(!result) {
      printf("Extension %s could not be loaded.\n", name);
   }
   return result;
}

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC) {
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	*hDC = GetDC(hWnd);

	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(*hDC, &pfd);
	if (!format) {
		cout << "No suitable pixel format could be found!" << endl;
	} else {
		if (!SetPixelFormat(*hDC, format, &pfd)) {
			cout << "Error setting OpenGL pixel format" << endl;
		}
	}

	// create and enable the render context (RC)
	*hRC = wglCreateContext(*hDC);
	if (!hRC) {
		cout << "OpenGL Context was not created successfully!" << endl;
		// GetLastError() will have error status
	}
	if (!wglMakeCurrent(*hDC, *hRC)) {
		cout << "Unable to set rendering context using wglMakeCurrent()!" << endl;
	}

	const char *extensions = (const char*) glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 ) {
		cout << "No swap control extension on this OS" << endl;
	} else {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		if( wglSwapIntervalEXT ) {
			// cout << "VSync enabled" << endl;
			wglSwapIntervalEXT(0);
		}
		glxSwapIntervalSGI = (PFNGLXSWAPINTERVALFARPROC)wglGetProcAddress( "glxSwapIntervalSGI" );
		if( glxSwapIntervalSGI ) {
			// cout << "VSync enabled" << endl;
			glxSwapIntervalSGI(0);
		}
	}
	const char *vendor = (const char*)glGetString(GL_VENDOR);
	const char *renderer = (const char*)glGetString(GL_RENDERER);
	const char *version = (const char*)glGetString(GL_VERSION);
	char buf[100];
	sprintf(buf, "OpenGL Vendor [%s]", vendor);
	logmsg(buf);
	sprintf(buf, "OpenGL Renderer [%s]", renderer);
	logmsg(buf);
	sprintf(buf, "OpenGL Version [%s]", version);
	logmsg(buf);
	logmsg("Loading OpenGL 1.3/1.5/2.0 extensions");
	// load 1.3 extensions
	glActiveTexture = (PFNGLACTIVETEXTUREPROC) wglLoadExtension("glActiveTexture");
	glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) wglLoadExtension("glSampleCoverage");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) wglLoadExtension("glCompressedTexImage2D");
	glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) wglLoadExtension("glCompressedTexSubImage2D");
	// load 1.5 extensions
	glGenBuffers = (PFNGLGENBUFFERSPROC) wglLoadExtension("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC) wglLoadExtension("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC) wglLoadExtension("glBufferData");
	glBufferSubData = (PFNGLBUFFERSUBDATAPROC) wglLoadExtension("glBufferSubData");
	glIsBuffer = (PFNGLISBUFFERPROC) wglLoadExtension("glIsBuffer");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglLoadExtension("glDeleteBuffers");
	// load extensions we need for opengl 2.0
    glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) wglLoadExtension("glBlendEquationSeparate");
	glDrawBuffers = (PFNGLDRAWBUFFERSPROC) wglLoadExtension("glDrawBuffers");
	glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) wglLoadExtension("glStencilOpSeparate");
	glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) wglLoadExtension("glStencilFuncSeparate");
	glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) wglLoadExtension("glStencilMaskSeparate");
	glAttachShader = (PFNGLATTACHSHADERPROC) wglLoadExtension("glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) wglLoadExtension("glBindAttribLocation");
	glCompileShader = (PFNGLCOMPILESHADERPROC) wglLoadExtension("glCompileShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglLoadExtension("glCreateProgram");
	glCreateShader = (PFNGLCREATESHADERPROC) wglLoadExtension("glCreateShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglLoadExtension("glDeleteProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC) wglLoadExtension("glDeleteShader");
	glDetachShader = (PFNGLDETACHSHADERPROC) wglLoadExtension("glDetachShader");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglLoadExtension("glDisableVertexAttribArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglLoadExtension("glEnableVertexAttribArray");
	glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) wglLoadExtension("glGetActiveAttrib");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) wglLoadExtension("glGetActiveUniform");
	glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) wglLoadExtension("glGetAttachedShaders");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglLoadExtension("glGetAttribLocation");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglLoadExtension("glGetProgramiv");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglLoadExtension("glGetProgramInfoLog");
	glGetShaderiv = (PFNGLGETSHADERIVPROC) wglLoadExtension("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglLoadExtension("glGetShaderInfoLog");
	glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) wglLoadExtension("glGetShaderSource");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglLoadExtension("glGetUniformLocation");
	glGetUniformfv = (PFNGLGETUNIFORMFVPROC) wglLoadExtension("glGetUniformfv");
	glGetUniformiv = (PFNGLGETUNIFORMIVPROC) wglLoadExtension("glGetUniformiv");
	glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) wglLoadExtension("glGetVertexAttribdv");
	glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) wglLoadExtension("glGetVertexAttribfv");
	glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) wglLoadExtension("glGetVertexAttribiv");
	glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) wglLoadExtension("glGetVertexAttribPointerv");
	glIsProgram = (PFNGLISPROGRAMPROC) wglLoadExtension("glIsProgram");
	glIsShader = (PFNGLISSHADERPROC) wglLoadExtension("glIsShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) wglLoadExtension("glLinkProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC) wglLoadExtension("glShaderSource");
	glUseProgram = (PFNGLUSEPROGRAMPROC) wglLoadExtension("glUseProgram");
	glUniform1f = (PFNGLUNIFORM1FPROC) wglLoadExtension("glUniform1f");
	glUniform2f = (PFNGLUNIFORM2FPROC) wglLoadExtension("glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC) wglLoadExtension("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC) wglLoadExtension("glUniform4f");
	glUniform1i = (PFNGLUNIFORM1IPROC) wglLoadExtension("glUniform1i");
	glUniform2i = (PFNGLUNIFORM2IPROC) wglLoadExtension("glUniform2i");
	glUniform3i = (PFNGLUNIFORM3IPROC) wglLoadExtension("glUniform3i");
	glUniform4i = (PFNGLUNIFORM4IPROC) wglLoadExtension("glUniform4i");
	glUniform1fv = (PFNGLUNIFORM1FVPROC) wglLoadExtension("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC) wglLoadExtension("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC) wglLoadExtension("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC) wglLoadExtension("glUniform4fv");
	glUniform1iv = (PFNGLUNIFORM1IVPROC) wglLoadExtension("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC) wglLoadExtension("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC) wglLoadExtension("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC) wglLoadExtension("glUniform4iv");
	glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) wglLoadExtension("glUniformMatrix2fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) wglLoadExtension("glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) wglLoadExtension("glUniformMatrix4fv");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) wglLoadExtension("glValidateProgram");
	glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) wglLoadExtension("glVertexAttrib1d");
	glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) wglLoadExtension("glVertexAttrib1dv");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) wglLoadExtension("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) wglLoadExtension("glVertexAttrib1fv");
	glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) wglLoadExtension("glVertexAttrib1s");
	glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) wglLoadExtension("glVertexAttrib1sv");
	glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) wglLoadExtension("glVertexAttrib2d");
	glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) wglLoadExtension("glVertexAttrib2dv");
	glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) wglLoadExtension("glVertexAttrib2f");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) wglLoadExtension("glVertexAttrib2fv");
	glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) wglLoadExtension("glVertexAttrib2s");
	glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) wglLoadExtension("glVertexAttrib2sv");
	glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) wglLoadExtension("glVertexAttrib3d");
	glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) wglLoadExtension("glVertexAttrib3dv");
	glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) wglLoadExtension("glVertexAttrib3f");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) wglLoadExtension("glVertexAttrib3fv");
	glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) wglLoadExtension("glVertexAttrib3s");
	glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) wglLoadExtension("glVertexAttrib3sv");
	glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) wglLoadExtension("glVertexAttrib4Nbv");
	glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) wglLoadExtension("glVertexAttrib4Niv");
	glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) wglLoadExtension("glVertexAttrib4Nsv");
	glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) wglLoadExtension("glVertexAttrib4Nub");
	glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) wglLoadExtension("glVertexAttrib4Nubv");
	glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) wglLoadExtension("glVertexAttrib4Nuiv");
	glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) wglLoadExtension("glVertexAttrib4Nusv");
	glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) wglLoadExtension("glVertexAttrib4bv");
	glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) wglLoadExtension("glVertexAttrib4d");
	glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) wglLoadExtension("glVertexAttrib4dv");
	glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) wglLoadExtension("glVertexAttrib4f");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) wglLoadExtension("glVertexAttrib4fv");
	glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) wglLoadExtension("glVertexAttrib4iv");
	glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) wglLoadExtension("glVertexAttrib4s");
	glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) wglLoadExtension("glVertexAttrib4sv");
	glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) wglLoadExtension("glVertexAttrib4ubv");
	glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) wglLoadExtension("glVertexAttrib4uiv");
	glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) wglLoadExtension("glVertexAttrib4usv");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglLoadExtension("glVertexAttribPointer");

	// now renderbuffer/framebuffer extensions
	glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglLoadExtension("glIsRenderbuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglLoadExtension("glBindRenderbuffer");
	glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglLoadExtension("glDeleteRenderbuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglLoadExtension("glGenRenderbuffers");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglLoadExtension("glRenderbufferStorage");
	glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglLoadExtension("glGetRenderbufferParameteriv");
	glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglLoadExtension("glIsFramebuffer");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglLoadExtension("glBindFramebuffer");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglLoadExtension("glDeleteFramebuffers");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglLoadExtension("glGenFramebuffers");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglLoadExtension("glCheckFramebufferStatus");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglLoadExtension("glFramebufferTexture2D");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglLoadExtension("glFramebufferRenderbuffer");
	glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglLoadExtension

("glGetFramebufferAttachmentParameteriv");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglLoadExtension("glGenerateMipmap");
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent(NULL, NULL );
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

#endif /* _WIN32 */
