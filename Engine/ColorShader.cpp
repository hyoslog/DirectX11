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

    // 버텍스 셰이더 코드 컴파일
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

    // 픽셀 셰이더 코드 컴파일
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

    // 정적 레이아웃 설명 설정
    //@warning 이 설정은 Model 클래스 및 셰이더의 VertexType 구조와 일치해야 함
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

    // 레이아웃의 요소 수를 가져옴
    const unsigned int elementsCount = (sizeof(polygonLayout) / sizeof(polygonLayout[0]));

    // 정점 입력 레이아웃 생성
    result = InDevice->CreateInputLayout(polygonLayout, elementsCount, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &Layout);
    if (FAILED(result) == true)
    {
        return false;
    }

    // 레이아웃이 생성되면 더 이상 필요하지 않으므로, 정점 및 픽셀 셰이더 버퍼 해제
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // 정점 셰이더에 있는 동적 매트릭스 상수 버퍼의 설명 설정
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(TMatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스할 수 있도록 상수 버퍼 포인터 생성
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
   // 오류 메세지 텍스트 버퍼에 대한 포인터를 가져옴
   const char* const compileErrors = static_cast<char*>(InErrorMessage->GetBufferPointer());

    // 오류 메세지 길이
   const unsigned long long bufferSize = InErrorMessage->GetBufferSize();

    // 파일에 오류 메세지 작성
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

    // 행렬을 셰이더로 보내기 전에 행렬을 전치(DX11 요구사항)
    OutWorldMatrix = XMMatrixTranspose(OutWorldMatrix);
    OutViewMatrix = XMMatrixTranspose(OutViewMatrix);
    OutProjectionMatrix = XMMatrixTranspose(OutProjectionMatrix);

    // 쓸 수 있도록 상수 버퍼 잠금
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    result = InDeviceContext->Map(MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result) == true)
	{
		return false;
	}

    // 상수 버퍼의 데이터에 대한 포인터
    TMatrixBufferType* dataPtr = static_cast<TMatrixBufferType*>(mappedResource.pData);

    // 행렬을 상수 버퍼에 복사
	dataPtr->World = OutWorldMatrix;
	dataPtr->View = OutViewMatrix;
	dataPtr->Projection = OutProjectionMatrix;

    // 상수 버퍼 잠금 해제
    InDeviceContext->Unmap(MatrixBuffer, 0);

    // 업데이트된 값으로 버텍스 셰이더에 상수 버퍼 설정
    InDeviceContext->VSSetConstantBuffers(0, 1, &MatrixBuffer);

    return true;
}

void CColorShader::RenderShader(ID3D11DeviceContext* InDeviceContext, const int InIndexCount)
{
    // 정점 입력 레이아웃 설정
    InDeviceContext->IASetInputLayout(Layout);

    // 삼각형을 렌더링하는 데 사용할 정점 및 픽셀 셰이더 설정
	InDeviceContext->VSSetShader(VertexShader, nullptr, 0);
	InDeviceContext->PSSetShader(PixelShader, nullptr, 0);

    // 삼각형 렌더링
    InDeviceContext->DrawIndexed(InIndexCount, 0, 0);
}