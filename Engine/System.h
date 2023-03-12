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
	// �����ϱ� ������ ���� ���α׷��� �ݺ��ǰ� ��� ���� ���α׷� ó���� �����ϴ� ��
	void Run();

public:
	// ���ø����̼��� ����Ǵ� ���� ���ø����̼����� ���۵Ǵ� Windows �ý��� �޼��� ó��
	LRESULT CALLBACK MessageHandler(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam);

private:
	// ���ø����̼ǿ� ���� ��� ó�� ����
	bool Frame();
	// �������� ����� â�� ����
	void InitializeWindows(int& OutScreenWidth, int& OutScreenHeight);
	// ȭ�� ������ �������� �ǵ����� ��ġ�� ���� �ڵ��� ����
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
// �Լ� ������Ÿ��
//////////////////////////////////////////////////////////////////////////
// Windows�� �޼����� ������ ȣ��Ǵ� �ݹ�
static LRESULT CALLBACK WndProc(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam);

//////////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////////
static CSystem* g_ApplicationHnadle = nullptr;