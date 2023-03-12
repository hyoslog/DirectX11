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

bool CColorShader::Render(ID3D11DeviceContext* InDeviceContext, const int InIndexCount, XMMATRIX& OutWorldMatrix, XMMATRIX& OutViewMatrix, XMMATRIX& OutProjectionMatrix)
{
    if (SetShaderParameters(InDeviceContext, OutWorldMatrix, OutViewMatrix, OutProjectionMatrix) == false)
    {
        return false;
    }

    RenderShader(InDeviceContext, InIndexCount);

	return true;
}

bool CColorShader::InitializeShader(ID3D11Device* InDevice, const HWND InHwnd, const wstring& InVsFileName, const wstring& InPsFileName)
{
    HRESULT result = S_OK;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ� ������
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(InVsFileName.c_str(), nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result) == true)
    {
        if (errorMessage != nullptr)
        {
            OutputShaderErrorMessage(errorMessage, InHwnd, InVsFileName);

            errorMessage->Release();
            errorMessage = nullptr;
        }
        else
        {
            MessageBox(InHwnd, InVsFileName.c_str(), L"Missing Shader File.", MB_OK);
        }

        return false;
    }

    // �ȼ� ���̴� �ڵ� ������
    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(InPsFileName.c_str(), nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result) == true)
    {
        if (errorMessage != nullptr)
        {
            OutputShaderErrorMessage(errorMessage, InHwnd, InPsFileName);

			errorMessage->Release();
			errorMessage = nullptr;
        }
        else
        {
            MessageBox(InHwnd, InPsFileName.c_str(), L"Missing Shader File.", MB_OK);
        }

        return false;
    }

    // ���ۿ��� ���ؽ� ���̴� ����
    result = InDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &VertexShader);
    if (FAILED(result) == true)
    {
        return false;
    }

    // ���ۿ��� �ȼ� ���̴� ����
    result = InDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &PixelShader);
    if (FAILED(result) == true)
    {
        return false;
    }

    // ���� ���̾ƿ� ���� ����
    //@warning �� ������ Model Ŭ���� �� ���̴��� VertexType ������ ��ġ�ؾ� ��
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // ���̾ƿ��� ��� ���� ������
    const unsigned int elementsCount = (sizeof(polygonLayout) / sizeof(polygonLayout[0]));

    // ���� �Է� ���̾ƿ� ����
    result = InDevice->CreateInputLayout(polygonLayout, elementsCount, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &Layout);
    if (FAILED(result) == true)
    {
        return false;
    }

    // ���̾ƿ��� �����Ǹ� �� �̻� �ʿ����� �����Ƿ�, ���� �� �ȼ� ���̴� ���� ����
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // ���� ���̴��� �ִ� ���� ��Ʈ���� ��� ������ ���� ����
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(TMatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� �׼����� �� �ֵ��� ��� ���� ������ ����
    result = InDevice->CreateBuffer(&matrixBufferDesc, nullptr, &MatrixBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

    return true;
}

void CColorShader::ShutdownShader()
{
    if (MatrixBuffer != nullptr)
    {
        MatrixBuffer->Release();
        MatrixBuffer = nullptr;
    }

	if (Layout != nullptr)
	{
        Layout->Release();
        Layout = nullptr;
	}

	if (PixelShader != nullptr)
	{
        PixelShader->Release();
        PixelShader = nullptr;
	}

	if (VertexShader != nullptr)
	{
        VertexShader->Release();
        VertexShader = nullptr;
	}
}

void CColorShader::OutputShaderErrorMessage(ID3D10Blob* InErrorMessage, const HWND InHwnd, const wstring& InShaderFileName)
{	
   // ���� �޼��� �ؽ�Ʈ ���ۿ� ���� �����͸� ������
   const char* const compileErrors = static_cast<char*>(InErrorMessage->GetBufferPointer());

    // ���� �޼��� ����
   const unsigned long long bufferSize = InErrorMessage->GetBufferSize();

    // ���Ͽ� ���� �޼��� �ۼ�
   ofstream fout;
   fout.open("shader_error.txt");
   for (unsigned long long bufferIndex = 0; bufferIndex < bufferSize; ++bufferIndex)
   {
       fout << compileErrors[bufferIndex];
   }
   fout.close();

   MessageBox(InHwnd, L"Error compiling shader. Check shader_error.txt for message.", InShaderFileName.c_str(), MB_OK);
}

bool CColorShader::SetShaderParameters(ID3D11DeviceContext* InDeviceContext, XMMATRIX& OutWorldMatrix, XMMATRIX& OutViewMatrix, XMMATRIX& OutProjectionMatrix)
{
    HRESULT result = S_OK;      

    // ����� ���̴��� ������ ���� ����� ��ġ(DX11 �䱸����)
    OutWorldMatrix = XMMatrixTranspose(OutWorldMatrix);
    OutViewMatrix = XMMatrixTranspose(OutViewMatrix);
    OutProjectionMatrix = XMMatrixTranspose(OutProjectionMatrix);

    // �� �� �ֵ��� ��� ���� ���
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    result = InDeviceContext->Map(MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result) == true)
	{
		return false;
	}

    // ��� ������ �����Ϳ� ���� ������
    TMatrixBufferType* dataPtr = static_cast<TMatrixBufferType*>(mappedResource.pData);

    // ����� ��� ���ۿ� ����
	dataPtr->World = OutWorldMatrix;
	dataPtr->View = OutViewMatrix;
	dataPtr->Projection = OutProjectionMatrix;

    // ��� ���� ��� ����
    InDeviceContext->Unmap(MatrixBuffer, 0);

    // ������Ʈ�� ������ ���ؽ� ���̴��� ��� ���� ����
    InDeviceContext->VSSetConstantBuffers(0, 1, &MatrixBuffer);

    return true;
}

void CColorShader::RenderShader(ID3D11DeviceContext* InDeviceContext, const int InIndexCount)
{
    // ���� �Է� ���̾ƿ� ����
    InDeviceContext->IASetInputLayout(Layout);

    // �ﰢ���� �������ϴ� �� ����� ���� �� �ȼ� ���̴� ����
	InDeviceContext->VSSetShader(VertexShader, nullptr, 0);
	InDeviceContext->PSSetShader(PixelShader, nullptr, 0);

    // �ﰢ�� ������
    InDeviceContext->DrawIndexed(InIndexCount, 0, 0);
}