#include <windows.h>
#include "glut.h"
#include "Cat.h"
#include "Lamborghini.h"

void OutputDebug(const char* Format)
{
	OutputDebugStringA(Format);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

HWND g_hWind = NULL;

HBITMAP hBitmap;
unsigned char* FrameBuffer;
void glInit();
void glSetup();
void glRender();

Cat S;
Lamborghini Lambor;

LONG W = 500;
LONG H = 500;
static RECT Region = { 0,0,W,H };
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof WNDCLASSEX;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);



	AdjustWindowRect(&Region, WS_OVERLAPPEDWINDOW, FALSE);
	FrameBuffer = new unsigned char[W * H  * 4]();

	g_hWind = CreateWindowEx
	(
		NULL,
		L"WindowClass1",
		L"dx11demo",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		Region.right - Region.left,
		Region.bottom - Region.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);


	ShowWindow(g_hWind, nCmdShow);

	glutInit(0, 0, W, H, FrameBuffer);
	//glInit();
	//glSetup();

	//S.Init();
	Lambor.Init();

	MSG msg;
	while (true)
	{
		//glRender();
		//S.Draw();
		Lambor.Draw();
		glutPresent();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		Sleep(10);
	}

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case  WM_CREATE:
	{
		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = W;
		bmi.bmiHeader.biHeight = H;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		hBitmap = CreateDIBSection(GetDC(g_hWind), &bmi, DIB_RGB_COLORS, (VOID**)&FrameBuffer, NULL, 0);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;// DC(可画图的内存对象) 的句柄
		HDC hdc;   // 通过窗口句柄获取该窗口的 DC
		hdc = BeginPaint(hWnd, &ps);
		HDC hDCMem = CreateCompatibleDC(hdc);
		HGDIOBJ hbmOld = SelectObject(hDCMem, hBitmap);
		BITMAP bm;
		GetObject(hBitmap, sizeof(bm), &bm);
		BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hDCMem, 0, 0, SRCCOPY);
		SelectObject(hDCMem, hbmOld);
		DeleteDC(hDCMem);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}break;
	}

	return	DefWindowProc(hWnd, message, wParam, lParam);
}
