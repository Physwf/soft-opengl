#include "glut.h"
#include "gl_pipeline.h"

#include <cstdio>
#include <cmath>

unsigned char* pFrameBuffer;
extern HWND g_hWind;

LONG X;
LONG Y;
LONG Width;
LONG Height;

void glutInit(int x, int y, int w, int h,unsigned char* fb)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
	pFrameBuffer = fb;
}

void glutPresent()
{
	gl_swap_frame_buffer();
	gl_copy_front_buffer(pFrameBuffer);
// 	for (LONG y = 0; y < Height; ++y)
// 	{
// 		for (LONG x = 0; x < Width; ++x)
// 		{
// 			if (x == y)
// 			{
// 				pFrameBuffer[4 * y * Width + x * 4 + 0] = 255;
// 				pFrameBuffer[4 * y * Width + x * 4 + 1] = 0;
// 				pFrameBuffer[4 * y * Width + x * 4 + 2] = 0;
// 			}
// 		}
// 	}
	RECT rect = { X,Y,Width,Height };
	InvalidateRect(g_hWind, &rect, false);
	UpdateWindow(g_hWind);
}

float* glutMatrixOrthoLH(float* pOut, float l, float r, float b, float t, float zn, float zf)
{
	float result[]
	{
		2.0f/(r-l),	0.0f,		0.0f,			-(r+l)/(r-l),
		0.0f,		2.0f /(t-b),0.0f,			-(t+b)/(t-b),
		0.0f,		0.0f,		1/(zf-zn),		-zn/(zf-zn),
		0.0f,		0.0f,		0.0f,			1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}


float* glutMatrixRotationX(float* pOut, float rad)
{
	float result[]
	{
		1.0f,			0.0f,				0.0f,				0.0f,
		0.0f,			std::cos(rad),		-std::sin(rad),		0.0f,
		0.0f,			std::sin(rad),		std::cos(rad),		0.0f,
		0.0f,			0.0f,				0.0f,				1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}

float* glutMatrixRotationY(float* pOut, float rad)
{
	float result[]
	{
		std::cos(rad),	0.0f,		std::sin(rad),				0.0f,
		0.0f,			1.0f,		0.0f,						0.0f,
		-std::sin(rad),	0.0f,		std::cos(rad),				0.0f,
		0.0f,			0.0f,		0.0f,						1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}

float* glutMatrixRotationZ(float* pOut, float rad)
{
	float result[]
	{
		std::cos(rad),	-std::sin(rad),		1.0f,				0.0f,
		0.0f,			std::cos(rad),		0.0f,				0.0f,
		std::sin(rad),	0.0f,				1.0f,				0.0f,
		0.0f,			0.0f,				0.0f,				1.0f,
	};
	for (int i = 0; i < 16; ++i)
	{
		pOut[i] = result[i];
	}
	return pOut;
}

void XLOG(const char* format, ...)
{
	char buffer[1024] = { 0 };
	va_list v_list;
	va_start(v_list, format);
	vsprintf_s(buffer, format, v_list);
	va_end(v_list);
	extern void OutputDebug(const char* Format);
	OutputDebug(buffer);
}

