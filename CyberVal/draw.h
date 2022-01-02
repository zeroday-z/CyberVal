#ifndef __DRAW_H__
#define __DRAW_H__

#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using namespace std;

enum TextAlignment { kLeft, kCenter, kRight };


class Draw
{
public:
	Draw()
	{
		isInitialized = false;
	}

	IDirect3DDevice9Ex* device;
	LPD3DXFONT font;
	void Init() { this->isInitialized = true; }
	bool IsInitialized() { return this->isInitialized; }

	int TextWidth(string text);
	void Text(int x, int y, string text, D3DCOLOR color, bool isBordered = false, TextAlignment eAlignment = TextAlignment::kLeft);
	void Line(float x, float y, float x2, float y2, D3DCOLOR color);
	void Border(float x, float y, float w, float h, D3DCOLOR color);
	void Rectangle(float x, float y, float w, float h, D3DCOLOR startColor, D3DCOLOR endColor = NULL, D3DCOLOR borderColor = NULL);
	void FPSCheck(std::string& str);
	IDirect3DDevice9Ex* GetDevice() { return this->device; }
private:
	struct Vertex
	{
		float x, y, z, h;
		D3DCOLOR color;
		float tu, tv;
		static DWORD FVF;
	};

	bool isInitialized;
	
};

#define opacity(v)				(255 * v) / 100

#define RedColor(a)				D3DCOLOR_ARGB(opacity(a), 255, 0, 0)
#define GreenColor(a)			D3DCOLOR_ARGB(opacity(a), 0, 255, 0)
#define BlueColor(a)			D3DCOLOR_ARGB(opacity(a), 0, 0, 255)
#define YellowColor(a)			D3DCOLOR_ARGB(opacity(a), 255, 255, 0)
#define OrangeColor(a)			D3DCOLOR_ARGB(opacity(a), 255, 125, 0)
#define WhiteColor(a)			D3DCOLOR_ARGB(opacity(a), 255, 255, 255)
#define BlackColor(a)			D3DCOLOR_ARGB(opacity(a), 0, 0, 0)

extern Draw draw;

#endif //__DRAW_H__