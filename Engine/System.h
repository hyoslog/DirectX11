#pragma once

//////////////////////////////////////////////////////////////////////////
// Define
//////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////////////////////////////////////////////////////////////////
// Include 
//////////////////////////////////////////////////////////////////////////
#include <Windows.h>

class CGraphics;
class CInput;
class CSystem final
{
public:
	CSystem() = default;
	CSystem(const CSystem&) = default;
	~CSystem() = default;

public:
	bool Initialize();
	void Shutdown();
	// 종료하기 전까지 응용 프로그램이 반복되고 모든 응용 프로그램 처리를 수행하는 곳
	void Run();

public:
	// 애플리케이션이 실행되는 동안 애플리케이션으로 전송되는 Windows 시스템 메세지 처리
	LRESULT CALLBACK MessageHandler(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam);

private:
	// 애플리케이션에 대한 모든 처리 수행
	bool Frame();
	// 렌더링에 사용할 창을 세팅
	void InitializeWindows(int& OutScreenWidth, int& OutScreenHeight);
	// 화면 설정을 정상으로 되돌리고 장치와 관련 핸들을 해제
	void ShutdownWindows();

private:
	LPCWSTR ApplicationName;
	HINSTANCE HInstance;
	HWND HWnd;

private:
	CInput* Input;
	CGraphics* Graphics;
};

//////////////////////////////////////////////////////////////////////////
// 함수 프로토타입
//////////////////////////////////////////////////////////////////////////
// Windows가 메세지를 보내면 호출되는 콜백
static LRESULT CALLBACK WndProc(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam);

//////////////////////////////////////////////////////////////////////////
// 전역
//////////////////////////////////////////////////////////////////////////
static CSystem* g_ApplicationHnadle = nullptr;