#include <cassert>
#include "D3D.h"
#include "Graphics.h"

CGraphics::CGraphics()
	: Direct3D(nullptr)
{

}

CGraphics::CGraphics(const CGraphics& InOther)
{

}

CGraphics::~CGraphics()
{

}

bool CGraphics::Initialize(const int InScreenWidth, const int InScreenHeight, const HWND InHwnd)
{
	// Direct3D ��ü ���� �� �ʱ�ȭ
	Direct3D = new CD3D;
	assert(Direct3D);
	if (Direct3D->Initialize(InScreenWidth, InScreenHeight, VSYNC_ENABLED, InHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR) == false)
	{
		MessageBox(InHwnd, L"Failed to Initialzie Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CGraphics::Shutdown()
{
	if (Direct3D != nullptr)
	{
		Direct3D->ShutDown();
		delete Direct3D;
		Direct3D = nullptr;
	}
}

bool CGraphics::Frame()
{
	if (Render() == false)
	{
		return false;
	}

	return true;
}

bool CGraphics::Render()
{
	// ���۸� ������ ����� ����
	Direct3D->BegineScene(0.0f, 1.0f, 1.0f, 1.0f);

	// �������� ����� ȭ�鿡 ǥ��
	Direct3D->EndScene();

	return true;
}