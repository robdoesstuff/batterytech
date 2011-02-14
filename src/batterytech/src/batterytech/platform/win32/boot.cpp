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
using namespace std;

#define CONSOLE 1
#define DEFAULT_WIDTH 480
#define DEFAULT_HEIGHT 800

#ifdef __MINGW32__
#define _LPCSTR LPCSTR
#else
#define _LPCSTR LPCWSTR
#endif

PFNGLCREATESHADERPROC 			glCreateShader = NULL;
PFNGLATTACHSHADERPROC 			glAttachShader = NULL;
PFNGLSHADERSOURCEPROC			glShaderSource = NULL;
PFNGLGETSHADERIVPROC			glGetShaderiv = NULL;
PFNGLCOMPILESHADERPROC          glCompileShader = NULL;
PFNGLDETACHSHADERPROC           glDetachShader = NULL;
PFNGLDELETESHADERPROC           glDeleteShader = NULL;
PFNGLGETSHADERINFOLOGPROC       glGetShaderInfoLog = NULL;

PFNGLCREATEPROGRAMPROC          glCreateProgram = NULL;
PFNGLLINKPROGRAMPROC            glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC           glGetProgramiv = NULL;
PFNGLUSEPROGRAMPROC             glUseProgram = NULL;
PFNGLDELETEPROGRAMPROC          glDeleteProgram = NULL;
PFNGLGETPROGRAMINFOLOGPROC      glGetProgramInfoLog = NULL;

PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC              glUniform1i = NULL;
PFNGLUNIFORM1FPROC              glUniform1f = NULL;
PFNGLVERTEXATTRIB4FPROC			glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray = NULL;
PFNGLBINDATTRIBLOCATIONPROC		glBindAttribLocation = NULL;

// WinMain
DWORD WINAPI StartThread(LPVOID iValue);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
BOOL leftButtonDown = FALSE;
BOOL quit = FALSE;
HWND hWnd;

extern "C" {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int iCmdShow) {
	WNDCLASS wc;
	MSG msg;
	DWORD dwExStyle; // Window Extended Style
	DWORD dwStyle; // Window Style

	if (CONSOLE) {
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
	wc.lpszClassName = (_LPCSTR)"DemoApp";
	RegisterClass(&wc);

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	RECT WindowRect;
	WindowRect.left = (long) 0;
	WindowRect.right = (long) DEFAULT_WIDTH;
	WindowRect.top = (long) 0;
	WindowRect.bottom = (long) DEFAULT_HEIGHT;
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
			(_LPCSTR)"DemoApp", // Class Name
			(_LPCSTR)"Batterytech Demo App", // Window Title
			WS_CLIPSIBLINGS | // Required Window Style
					WS_CLIPCHILDREN | // Required Window Style
					WS_VISIBLE | dwStyle, // Selected Window Style
					dwWidth / 2 - DEFAULT_WIDTH / 2, dwHeight /2 - DEFAULT_HEIGHT / 2, // Window Position
			WindowRect.right - WindowRect.left, // Calculate Adjusted Window Width
			WindowRect.bottom - WindowRect.top, // Calculate Adjusted Window Height
			NULL, // No Parent Window
			NULL, // No Menu
			hInstance, // Instance
			NULL);

	HANDLE hThread1;
	DWORD dwGenericThread;
	hThread1 = CreateThread(NULL, 0, StartThread, NULL, 0, &dwGenericThread);
	if (!hThread1) {
		cout << "Error creating thread" << endl;
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
	}
	WaitForSingleObject(hThread1, INFINITE);
	// destroy the window explicitly
	DestroyWindow(hWnd);
	return msg.wParam;
}

}

DWORD WINAPI StartThread(LPVOID iValue) {
	cout << "Starting Game Thread" << endl;
	GraphicsConfiguration *gConfig;
	HGLRC hRC;
	HDC hDC;
	// enable OpenGL for the window
	cout << "Enabling OpenGL" << endl;
	EnableOpenGL(hWnd, &hDC, &hRC);
	gConfig = new GraphicsConfiguration;
	gConfig->supportsHWmipmapgen = true;
	gConfig->supportsUVTransform = true;
	gConfig->supportsVBOs = true;
	// what modern PC video card doesn't support shaders?
	if (glCreateShader) {
		logmsg("Shaders supported");
		gConfig->supportsShaders = true;
	} else {
		logmsg("Shaders not supported");
		gConfig->supportsShaders = false;
	}
	btInit(gConfig, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	DWORD time = timeGetTime();
	DWORD oldTime = time;
	while (!quit) {
		//cout << "Ticking" << endl;
		oldTime = time;
		time = timeGetTime();
		btUpdate((time - oldTime) / 1000.0f);
		btDraw();
		SwapBuffers(hDC);
	}
	// stop sound process before releasing app
	btRelease();
	// shutdown OpenGL
	DisableOpenGL(hWnd, hDC, hRC);
	delete gConfig;
	gConfig = NULL;
	return 0;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	int x,y;
	switch (message) {
	case WM_CHAR:
		btKeyPressed(wParam);
		return 0;
	case WM_KEYUP:
		btKeyUp(wParam);
		return 0;
	case WM_KEYDOWN:
		btKeyDown(wParam);
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
	SetPixelFormat(*hDC, format, &pfd);

	// create and enable the render context (RC)
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);

	const char *extensions = (const char*) glGetString( GL_EXTENSIONS );

	if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 ) {
		cout << "No swap control extension on this OS" << endl;
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	} else {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
		if( wglSwapIntervalEXT ) {
			cout << "VSync enabled" << endl;
			wglSwapIntervalEXT(1);
		}
		glxSwapIntervalSGI = (PFNGLXSWAPINTERVALFARPROC)wglGetProcAddress( "glxSwapIntervalSGI" );
		if( glxSwapIntervalSGI ) {
			cout << "VSync enabled" << endl;
			glxSwapIntervalSGI(1);
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
	logmsg("Loading OpenGL 2.0 extensions");
	// load extensions we need for opengl 2.0
    glCreateShader             = (PFNGLCREATESHADERPROC)        wglLoadExtension("glCreateShader");
    glAttachShader             = (PFNGLATTACHSHADERPROC)        wglLoadExtension("glAttachShader");
    glShaderSource             = (PFNGLSHADERSOURCEPROC)        wglLoadExtension("glShaderSource");
    glGetShaderiv              = (PFNGLGETSHADERIVPROC)         wglLoadExtension("glGetShaderiv");
    glCompileShader            = (PFNGLCOMPILESHADERPROC)       wglLoadExtension("glCompileShader");
    glDetachShader             = (PFNGLDETACHSHADERPROC)        wglLoadExtension("glDetachShader");
    glDeleteShader             = (PFNGLDELETESHADERPROC)        wglLoadExtension("glDeleteShader");
    glGetShaderInfoLog         = (PFNGLGETSHADERINFOLOGPROC)    wglLoadExtension("glGetShaderInfoLog");
    glCreateProgram            = (PFNGLCREATEPROGRAMPROC)       wglLoadExtension("glCreateProgram");
    glLinkProgram              = (PFNGLLINKPROGRAMPROC)         wglLoadExtension("glLinkProgram");
    glGetProgramiv             = (PFNGLGETPROGRAMIVPROC)        wglLoadExtension("glGetProgramiv");
    glUseProgram               = (PFNGLUSEPROGRAMPROC)          wglLoadExtension("glUseProgram");
    glDeleteProgram            = (PFNGLDELETEPROGRAMPROC)       wglLoadExtension("glDeleteProgram");
    glGetProgramInfoLog        = (PFNGLGETPROGRAMINFOLOGPROC)   wglLoadExtension("glGetProgramInfoLog");
    glGetUniformLocation       = (PFNGLGETUNIFORMLOCATIONPROC)  wglLoadExtension("glGetUniformLocation");
    glUniform1i                = (PFNGLUNIFORM1IPROC)           wglLoadExtension("glUniform1i");
    glUniform1f                = (PFNGLUNIFORM1FPROC)           wglLoadExtension("glUniform1f");
    glVertexAttrib4f           = (PFNGLVERTEXATTRIB4FPROC)         wglLoadExtension("glVertexAttrib4f");
    glVertexAttribPointer      = (PFNGLVERTEXATTRIBPOINTERPROC)         wglLoadExtension("glVertexAttribPointer");
    glEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)       wglLoadExtension("glEnableVertexAttribArray");
    glDisableVertexAttribArray  = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)    wglLoadExtension("glDisableVertexAttribArray");
    glBindAttribLocation 		= (PFNGLBINDATTRIBLOCATIONPROC) 		wglLoadExtension("glBindAttribLocation");
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent(NULL, NULL );
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

#endif /* _WIN32 */
