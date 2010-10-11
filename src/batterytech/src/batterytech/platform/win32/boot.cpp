#ifdef _WIN32

#include "resources.h"
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "../../batterytech.h"
#include "../../render/GraphicsConfiguration.h"
using namespace std;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 480

// WinMain
DWORD WINAPI StartThread(LPVOID iValue);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
BOOL leftButtonDown = FALSE;
BOOL quit = FALSE;
HWND hWnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int iCmdShow) {
	WNDCLASS wc;
	MSG msg;
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
	wc.lpszClassName = "BatteryTech";
	RegisterClass(&wc);

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW;
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
	hWnd = CreateWindowEx(dwExStyle, // Extended Style For The Window
			"BatteryTech", // Class Name
			"BatteryTech", // Window Title
			WS_CLIPSIBLINGS | // Required Window Style
					WS_CLIPCHILDREN | // Required Window Style
					WS_VISIBLE | dwStyle, // Selected Window Style
			0, 0, // Window Position
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
	// shutdown OpenGL
	DisableOpenGL(hWnd, hDC, hRC);
	delete gConfig;
	return 0;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	int x,y;
	switch (message) {

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

	case WM_KEYDOWN:
		switch (wParam) {

		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;

		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

}

// Enable OpenGL

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
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat(*hDC, &pfd);
	SetPixelFormat(*hDC, format, &pfd);

	// create and enable the render context (RC)
	*hRC = wglCreateContext(*hDC);
	wglMakeCurrent(*hDC, *hRC);

}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) {
	wglMakeCurrent(NULL, NULL );
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

#endif /* _WIN32 */
