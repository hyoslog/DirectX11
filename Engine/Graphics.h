#pragma once

//////////////////////////////////////////////////////////////////////////
// Include 
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

//////////////////////////////////////////////////////////////////////////
// 클래스
//////////////////////////////////////////////////////////////////////////
// 그래픽 렌더링 처리
class CD3D;
class CGraphics final
{
public:
	CGraphics();
	CGraphics(const CGraphics& InOther);
	~CGraphics();

public:
	bool Initialize(const int InScreenWidth, const int InScreenHeight, const HWND InHwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	CD3D* Direct3D;
};