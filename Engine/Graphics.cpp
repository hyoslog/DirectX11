#include <cassert>
#include "Camera.h"
#include "ColorShader.h"
#include "D3D.h"
#include "Graphics.h"
#include "Model.h"

bool CGraphics::Initialize(const int InScreenWidth, const int InScreenHeight, const HWND InHwnd)
{
	bool bResult = true;

	// Direct3D 객체 생성 및 초기화
	Direct3D = new CD3D();
	assert(Direct3D);
	bResult = Direct3D->Initialize(InScreenWidth, InScreenHeight, VSYNC_ENABLED, InHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (bResult == false)
	{
		MessageBox(InHwnd, L"Failed to initialzie Direct3D.", L"Error", MB_OK);
		return false;
	}

	Camera = new CCamera();
	assert(Camera);
	Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));

	Model = new CModel();
	assert(Model);
	bResult = Model->Initialize(Direct3D->GetDevice());
	if (bResult == false)
	{
		MessageBox(InHwnd, L"Failed to initialize the Model.", L"Error", MB_OK);
		return false;
	}

	ColorShader = new CColorShader();
	assert(ColorShader);
	bResult = ColorShader->Initialize(Direct3D->GetDevice(), InHwnd);
	if (bResult == false)
	{
		MessageBox(InHwnd, L"Failed to initialize ColorShader.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void CGraphics::Shutdown()
{
	if (ColorShader != nullptr)
	{
		ColorShader->Shutdown();
		delete ColorShader;
		ColorShader = nullptr;
	}

	if (Model != nullptr)
	{
		Model->Shutdown();
		delete Model;
		Model = nullptr;
	}

	if (Camera != nullptr)
	{
		delete Camera;
		Camera = nullptr;
	}

	if (Direct3D != nullptr)
	{
		Direct3D->Shutdown();
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
	// 버퍼를 지워서 장면을 시작
	Direct3D->BegineScene(0.0f, 0.0f, 0.0f, 1.0f);

	Camera->Render();

	XMMATRIX worldMatrix = {};
	Direct3D->GetWorldMatrix(worldMatrix);

	XMMATRIX viewdMatrix = {};
	Camera->GetViewMatrix(viewdMatrix);

	XMMATRIX projectionMatrix = {};
	Direct3D->GetPojectionMatrix(projectionMatrix);

	Model->Render(Direct3D->GetDeviceContext());

	if (ColorShader->Render(Direct3D->GetDeviceContext(), Model->GetIndexCount(), worldMatrix, viewdMatrix, projectionMatrix) == false)
	{
		return false;
	}

	// 렌더링된 장면을 화면에 표시
	Direct3D->EndScene();

	return true;
}