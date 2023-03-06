#pragma once

//////////////////////////////////////////////////////////////////////////
// Libraray
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//////////////////////////////////////////////////////////////////////////
// Å¬·¡½º
//////////////////////////////////////////////////////////////////////////
class CD3D final
{
public:
	CD3D();
	CD3D(const CD3D& InOther) = default;
	~CD3D();

public:
	bool Initialize(const unsigned int InScreenWidth, const unsigned int InScreenHeight, const bool InbVsync, const HWND InHwnd, const bool InbFullScreen, const float InScreenDepth, const float InScreenNear);
	void ShutDown();

	void BegineScene(const float InRed, const float InGreen, const float InBlue, const float InAlpha);
	void EndScene();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	void GetPojectionMatrix(XMMATRIX& OutProjectionMatrix);
	void GetWorldMatrix(XMMATRIX& OutWorldMatrix);
	void GetOrthoMatrix(XMMATRIX& OutOrthoMatrix);

	void GetVideoCardInfo(char* OutCardName, int& OutMemory);

private:
	bool bVsyncEnabled;
	unsigned int VideoCardMemory;
	char VideoCardDescription[128];
	IDXGISwapChain* SwapChain;
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11Texture2D* DepthStencilBuffer;
	ID3D11DepthStencilState* DepthStencilState;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RasterizerState* RasterState;
	XMMATRIX ProjectionMatrix;
	XMMATRIX WorldMatrix;
	XMMATRIX OrthoMatrix;
};