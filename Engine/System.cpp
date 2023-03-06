//////////////////////////////////////////////////////////////////////////
// ��� 
//////////////////////////////////////////////////////////////////////////
#include <cassert>
#include "Graphics.h"
#include "Input.h"
#include "System.h"

bool CSystem::Initialize()
{
	bool result = false;

	int screenWidth = 0;
	int screenHeight = 0;

	// ȭ�� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	// �Է� ��ü ���� �� �ʱ�ȭ
	Input = new CInput;
	assert(Input);
	Input->Initialize();

	// �׷��� ��ü ���� �� �ʱ�ȭ
	Graphics = new CGraphics;
	assert(Graphics);
	result = Graphics->Initialize(screenWidth, screenHeight, HWnd);

	if (result == false)
	{
		return false;
	}

	return true;
}

void CSystem::Shutdown()
{
	if (Graphics != nullptr)
	{
		Graphics->Shutdown();
		delete Graphics;
		Graphics = nullptr;
	}

	if (Input != nullptr)
	{
		delete Input;
		Input = nullptr;
	}

	// ȭ�� ����
	ShutdownWindows();
}

void CSystem::Run()
{
	bool bExit = false;

	MSG message = {};

	// Windows�� ����ڷκ��� ���� �޼����� ���� ������ �ݺ�
	while (bExit == false)
	{
		// Windows �޼��� ó��
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Windows�� ���� ���α׷��� �����϶�� ��ȣ�� ������ ����
		if (message.message == WM_QUIT)
		{
			bExit = true;
		}
		else
		{
			// �׷��� ������ ������ ó���� ����
			// ������ ó�� ����� �����϶�� ��ȣ��� ����
			if (Frame() == false)
			{
				bExit = true;
			}
		}
	}
}

bool CSystem::Frame()
{
	bool result = true;

	if (Input->IsKeyDown(VK_ESCAPE) == true)
	{
		result = false;
	}
	else if (Graphics->Frame() == false)
	{
		result = false;
	}

	return result;
}

LRESULT CALLBACK CSystem::MessageHandler(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam)
{
	switch (InMsg)
	{
	case WM_KEYDOWN:
	{
		Input->KeyDown(static_cast<unsigned int>(InWParam));
		return 0;
	}
	break;
	case WM_KEYUP:
	{
		Input->KeyUp(static_cast<unsigned int>(InWParam));
		return 0;
	}
	break;
	default:
	{
		return DefWindowProc(InHWND, InMsg, InWParam, InLParam);
	}
	break;
	}
}

void CSystem::InitializeWindows(int& OutScreenWidth, int& OutScreenHeight)
{
	g_ApplicationHnadle = this;
	HInstance = GetModuleHandle(nullptr);
	ApplicationName = L"Engine";

	WNDCLASSEX wc = {};
	wc.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC);
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.hInstance = HInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);	

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN == true)
	{
		// ȭ�� �ػ� ��������
		OutScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		OutScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		posX = 0;
		posY = 0;

		DEVMODE screen_mode = {};
		screen_mode.dmSize = sizeof(screen_mode);		
		screen_mode.dmPelsWidth = static_cast<unsigned long>(OutScreenWidth);
		screen_mode.dmPelsHeight = static_cast<unsigned long>(OutScreenHeight);
		screen_mode.dmBitsPerPel = 32; // ���÷��� ����̽��� �� �ػ��� �ȼ��� ��Ʈ ����
		screen_mode.dmFields = (DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT);
		ChangeDisplaySettings(&screen_mode, CDS_FULLSCREEN);		
	}
	else
	{
		OutScreenWidth = 800;
		OutScreenHeight = 600;

		// ȭ�� �߾ӿ� ��ġ
		posX = ((GetSystemMetrics(SM_CXSCREEN) - OutScreenWidth) / 2);
		posY = ((GetSystemMetrics(SM_CYSCREEN) - OutScreenHeight) / 2);				
	}

	HWnd = CreateWindowEx(WS_EX_APPWINDOW, ApplicationName, ApplicationName, (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP), posX, posY, OutScreenWidth, OutScreenHeight, nullptr, nullptr, HInstance, nullptr);

	// â�� ����, ���� ��Ŀ���� ����
	ShowWindow(HWnd, SW_SHOW);
	SetForegroundWindow(HWnd);
	SetFocus(HWnd);

	// ���콺 Ŀ���� ����
	ShowCursor(false);
}

void CSystem::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN == true)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// â�� ����
	DestroyWindow(HWnd);
	HWnd = nullptr;

	// ���ø����̼� �ν��Ͻ� ����
	UnregisterClass(ApplicationName, HInstance);
	HInstance = nullptr;
	
	g_ApplicationHnadle = nullptr;
}

LRESULT CALLBACK WndProc(const HWND InHWND, const UINT InMsg, const WPARAM InWParam, const LPARAM InLParam)
{
	switch (InMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	default:
	{
		return g_ApplicationHnadle->MessageHandler(InHWND, InMsg, InWParam, InLParam);
	}
	break;
	}
}