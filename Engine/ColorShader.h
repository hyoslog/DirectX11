#pragma once

//////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

// GPU에 있는 3D 모델을 그리기 위해 HLSL 셰이더를 호출
class CColorShader
{
private:
	struct TMatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	CColorShader() = default;
	CColorShader(const CColorShader& InOther) = default;
	~CColorShader() = default;

public:
	bool Initialize(ID3D11Device* InDevice, const HWND InHwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* InDeviceContext, const int InIndexCount, const XMMATRIX& InWorldMatrix, const XMMATRIX& InViewMatrix, const XMMATRIX& InProjectionMatrix);

private:
	bool InitializeShader(ID3D11Device* InDevice, const HWND InHwnd, const wstring& InVsFileName, const wstring& InPsFileName);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* InErrorMessage, const HWND InHwnd, const wstring& InShaderFileName);

	bool SetShaderParameters(ID3D11DeviceContext* InDeviceContext, const XMMATRIX& InWorldMatrix, const XMMATRIX& InViewMatrix, const XMMATRIX& InProjectionMatrix);
	void RenderShader(ID3D11DeviceContext* InDeviceContext, const int InIndexCount);

private:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* Layout;
	ID3D11Buffer* MatrixBuffer;
};