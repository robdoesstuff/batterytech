
#ifdef _WIN32

#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "../../batterytech.h"
using namespace std;

// WinMain
DWORD WINAPI StartThread(LPVOID iValue);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
BOOL leftButtonDown = FALSE;
BOOL quit = FALSE;
HWND hWnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int iCmdShow)
{
	WNDCLASS wc;
	MSG msg;

	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "BatteryTech";
	RegisterClass( &wc );

	// create main window
	hWnd = CreateWindow(
		"BatteryTech", "BatteryTech",
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		0, 0, 800, 480,
		NULL, NULL, hInstance, NULL );


	HANDLE hThread1;
	DWORD dwGenericThread;
	hThread1 = CreateThread(NULL,0,StartThread,NULL,0,&dwGenericThread);
	if (!hThread1) {
		cout << "Error creating thread" << endl;
	}
	// program main loop
	while ( !quit ) {
		// check for messages
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  ) {
			// handle or dispatch messages
			if ( msg.message == WM_QUIT ) {
				quit = TRUE;
			} else {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}
	}
	WaitForSingleObject(hThread1,INFINITE);
	// destroy the window explicitly
	DestroyWindow( hWnd );
	return msg.wParam;
}

DWORD WINAPI StartThread(LPVOID iValue) {
	cout << "Starting Game Thread" << endl;
	HGLRC hRC;
	HDC hDC;
	// enable OpenGL for the window
	cout << "Enabling OpenGL" << endl;
	EnableOpenGL( hWnd, &hDC, &hRC );
	btInit(800, 480);
	DWORD time = timeGetTime();
	DWORD oldTime = time;
	while (!quit) {
		//cout << "Ticking" << endl;
		oldTime = time;
		time = timeGetTime();
		btUpdate((time - oldTime) / 1000.0f);
		btDraw();
		SwapBuffers( hDC );
	}
	// shutdown OpenGL
	DisableOpenGL( hWnd, hDC, hRC );
	return 0;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_LBUTTONDOWN:
		leftButtonDown = TRUE;
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		btSetPointerState(0, true, x, y);
		return 0;
	case WM_LBUTTONUP:
		leftButtonDown = FALSE;
		btSetPointerState(0, false, 0, 0);
		return 0;
	case WM_MOUSEMOVE:
		if (leftButtonDown) {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			btSetPointerState(0, true, x, y);
		}
		return 0;
	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		switch ( wParam )
		{

		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;

		}
		return 0;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );

	}

}

// Enable OpenGL

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	*hDC = GetDC( hWnd );

	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, format, &pfd );

	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );

}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}

#endif /* _WIN32 */
