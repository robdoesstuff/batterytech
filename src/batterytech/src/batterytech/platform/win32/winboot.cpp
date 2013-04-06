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
		btGetContext()->gConfig->supportsShaders = TRUE;
	} else {
		logmsg("Shaders not supported");
		btGetContext()->gConfig->supportsShaders = FALSE;
		btGetContext()->gConfig->useShaders = FALSE;
	}
	BOOL fbosSupported = (glGenFramebuffers != NULL);
	if (fbosSupported) {
		logmsg("FBOs supported");
		btGetContext()->gConfig->supportsFBOs = TRUE;
	} else {
		logmsg("FBOs not supported");
		btGetContext()->gConfig->supportsFBOs = FALSE;
	}
	if (GLEW_VERSION_3_0 || GLEW_ARB_texture_float) {
		logmsg("Float textures supported");
		btGetContext()->gConfig->supportsFloatTextures = TRUE;
	}
	if (GLEW_VERSION_1_4 || GLEW_ARB_depth_texture) {
		logmsg("Depth textures supported");
		btGetContext()->gConfig->supportsDepthTextures = TRUE;
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
		char buf[512];
		sprintf(buf, "Extension %s could not be loaded.\n", name);
		logmsg(buf);
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
	logmsg("Loading OpenGL extensions");
	glewInit();
	if (!glCreateShader) {
		if (GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_shading_language_100) {
			logmsg("Using ARB Shader Objects");
			glCreateShader = glCreateShaderObjectARB;
			glShaderSource = glShaderSourceARB;
			glCompileShader = glCompileShaderARB;
			glGetShaderiv = glGetObjectParameterivARB;
			glGetProgramiv = glGetObjectParameterivARB;
			glGetShaderInfoLog = glGetInfoLogARB;
			glGetProgramInfoLog = glGetInfoLogARB;
			glDeleteProgram = glDeleteObjectARB;
			glDeleteShader = glDeleteObjectARB;
			glUseProgram = glUseProgramObjectARB;
			glAttachShader = glAttachObjectARB;
			glCreateProgram = glCreateProgramObjectARB;
			glValidateProgram = glValidateProgramARB;
			glLinkProgram = glLinkProgramARB;
			glBindAttribLocation = glBindAttribLocationARB;
			glDetachShader = glDetachObjectARB;
			glDisableVertexAttribArray = glDisableVertexAttribArrayARB;
			glEnableVertexAttribArray = glEnableVertexAttribArrayARB;
			glGetActiveAttrib = glGetActiveAttribARB;
			glGetActiveUniform = glGetActiveUniformARB;
			glGetAttachedShaders = glGetAttachedObjectsARB;
			glUniform1f = glUniform1fARB;
			glUniform1fv = glUniform1fvARB;
			glUniform1i = glUniform1iARB;
			glUniform1iv = glUniform1ivARB;
			glUniform2f = glUniform2fARB;
			glUniform2fv = glUniform2fvARB;
			glUniform2i = glUniform2iARB;
			glUniform2iv = glUniform2ivARB;
			glUniform3f = glUniform3fARB;
			glUniform3fv = glUniform3fvARB;
			glUniform3i = glUniform3iARB;
			glUniform3iv = glUniform3ivARB;
			glUniform4f = glUniform4fARB;
			glUniform4fv = glUniform4fvARB;
			glUniform4i = glUniform4iARB;
			glUniform4iv = glUniform4ivARB;
			glUniformMatrix2fv = glUniformMatrix2fvARB;
			glUniformMatrix3fv = glUniformMatrix3fvARB;
			glUniformMatrix4fv = glUniformMatrix4fvARB;
		}
	}
	if (!glGenFramebuffers) {
		if (GLEW_EXT_framebuffer_object) {
			logmsg("Using EXT Framebuffer");
			glGenFramebuffers = glGenFramebuffersEXT;
			glGenRenderbuffers = glGenRenderbuffersEXT;
			glBindRenderbuffer = glBindRenderbufferEXT;
			glRenderbufferStorage = glRenderbufferStorageEXT;
			glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
			glFramebufferTexture2D = glFramebufferTexture2DEXT;
			glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
			glBindFramebuffer = glBindFramebufferEXT;
			glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
			glIsRenderbuffer = glIsRenderbufferEXT;
			glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
			glIsFramebuffer = glIsFramebufferEXT;
			glDeleteFramebuffers = glDeleteFramebuffersEXT;
			glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
			glGetFramebufferParameteriv = glGetFramebufferParameterivEXT;
			glGenerateMipmap = glGenerateMipmapEXT;
		}
	}
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent(NULL, NULL );
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

#endif /* _WIN32 */
