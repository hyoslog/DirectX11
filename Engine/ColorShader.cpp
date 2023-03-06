#include "ColorShader.h"

bool CColorShader::Initialize(ID3D11Device* InDevice, const HWND InHwnd)
{
    wstring vsFileName = L"Color.vs";
    wstring psFileName = L"Color.ps";

    if (InitializeShader(InDevice, InHwnd, vsFileName, psFileName) == false)
    {
        return false;
    }

    return true;
}

void CColorShader::Shutdown()
{
    ShutdownShader();

    return;
}

bool CColorShader::Render(const ID3D11DeviceContext* InDeviceContext, const int InIndexCount, const XMMATRIX& InWorldMatrix, const XMMATRIX& InViewMatrix, const XMMATRIX& InProjectionMatrix)
{
    if (SetShaderParameters(InDeviceContext, InWorldMatrix, InViewMatrix, InProjectionMatrix) == false)
    {
        return false;
    }

    RenderShader(InDeviceContext, InIndexCount);

	return false;
}

bool CColorShader::InitializeShader(ID3D11Device* InDevice, const HWND InHwnd, const wstring& InVsFileName, const wstring& InPsFileName)
{
    HRESULT result = S_OK;
    ID3D10Blob* errorMessage = nullptr;

    // 버텍스 셰이더 코드 컴파일
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(InVsFileName.c_str(), nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result) == true)
    {
        if (errorMessage != nullptr)
        {
            OutputShaderErrorMessage(errorMessage, InHwnd, InVsFileName);
        }
        else
        {
            MessageBox(InHwnd, InVsFileName.c_str(), L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 픽셀 셰이더 코드 컴파일
    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(InPsFileName.c_str(), nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result) == true)
    {
        if (errorMessage != nullptr)
        {
            OutputShaderErrorMessage(errorMessage, InHwnd, InPsFileName);
        }
        else
        {
			MessageBox(InHwnd, InPsFileName.c_str(), L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼에서 버텍스 셰이더 생성
    result = InDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &VertexShader);
    if (FAILED(result) == true)
    {
        return false;
    }

    // 버퍼에서 픽셀 셰이더 생성
	result = InDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &PixelShader);
	if (FAILED(result) == true)
	{
		return false;
	}

    return true;
}

void CColorShader::ShutdownShader()
{
}

void CColorShader::OutputShaderErrorMessage(ID3D10Blob*, HWND, const wstring& InShaderFileName)
{
}

bool CColorShader::SetShaderParameters(const ID3D11DeviceContext* const InDeviceContext, const XMMATRIX& InWorldMatrix, const XMMATRIX& InViewMatrix, const XMMATRIX& InProjectionMatrix)
{
    return false;
}

void CColorShader::RenderShader(const ID3D11DeviceContext* const InDeviceContext, const int InIndexCount)
{
}