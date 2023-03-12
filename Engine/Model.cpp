#include "Model.h"

bool CModel::Initialize(ID3D11Device* InDevice)
{
    if (InitializeBuffers(InDevice) == false)
    {
        return false;
    }

    return true;
}

void CModel::Shutdown()
{
    ShutdownBuffers();
}

void CModel::Render(ID3D11DeviceContext* InDeviceContext)
{
    RenderBuffers(InDeviceContext);
}

bool CModel::InitializeBuffers(ID3D11Device* InDevice)
{
    VertexCount = 3;
    IndexCount = 3;

    TVertexType* vertices = new TVertexType[VertexCount]();
    if (vertices == nullptr)
    {
        return false;
    }

    unsigned long* indices = new unsigned long[IndexCount]();
    if (indices == nullptr)
    {
        return false;
    }

    HRESULT result = S_OK;

    // 시계 방향 순서로 정점 생성
    // 왼쪽 아래
	vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    // 가운데 위
	vertices[1].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// 오른쪽 아래
	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // 인덱스 배열에 정점 데이터 세팅
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

    // 정점 버퍼 설명 설정
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = (sizeof(TVertexType) * VertexCount); // 버퍼 크기
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 버퍼 유형
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 하위 리소스 구조에 정점 데이터에 대한 포인터 제공
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 정점 버퍼 생성
    result = InDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
    if (FAILED(result) == true)
    {
        return false;
    }

    // 인덱스 버퍼 설명 설정
	D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = (sizeof(TVertexType) * IndexCount); // 버퍼 크기
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 버퍼 유형
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 하위 리소스 구조에 인덱스 데이터에 대한 포인터 제겅
	D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼 생성
	result = InDevice->CreateBuffer(&indexBufferDesc, &indexData, &IndexBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

    // 정점 버퍼와 인덱스 버퍼가 생성된 후에는 데이터가 버퍼에 복사되었기 때문에 더 이상 필요하지 않은 정점 및 인덱스 배열 삭제
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void CModel::ShutdownBuffers()
{
    if (IndexBuffer != nullptr)
    {
        IndexBuffer->Release();
        IndexBuffer = nullptr;
    }

	if (VertexBuffer != nullptr)
	{
        VertexBuffer->Release();
        VertexBuffer = nullptr;
	}
}

void CModel::RenderBuffers(ID3D11DeviceContext* InDeviceContext)
{
	const unsigned int stride = sizeof(TVertexType);
	const unsigned int offset = 0;

    // 렌더링할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정
    InDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

    // 렌더링될 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정
    InDeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 이 정점 버퍼에서 렌더링되어야 하는 프리미티브 유형을 설정
    InDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 삼각형
}
