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

    // �ð� ���� ������ ���� ����
    // ���� �Ʒ�
	vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    // ��� ��
	vertices[1].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	// ������ �Ʒ�
	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // �ε��� �迭�� ���� ������ ����
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

    // ���� ���� ���� ����
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = (sizeof(TVertexType) * VertexCount); // ���� ũ��
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ����
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // ���� ���ҽ� ������ ���� �����Ϳ� ���� ������ ����
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ���� ����
    result = InDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
    if (FAILED(result) == true)
    {
        return false;
    }

    // �ε��� ���� ���� ����
	D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = (sizeof(TVertexType) * IndexCount); // ���� ũ��
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // ���� ����
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // ���� ���ҽ� ������ �ε��� �����Ϳ� ���� ������ ����
	D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ���� ����
	result = InDevice->CreateBuffer(&indexBufferDesc, &indexData, &IndexBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

    // ���� ���ۿ� �ε��� ���۰� ������ �Ŀ��� �����Ͱ� ���ۿ� ����Ǿ��� ������ �� �̻� �ʿ����� ���� ���� �� �ε��� �迭 ����
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

    // �������� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ ����
    InDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

    // �������� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ ����
    InDeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // �� ���� ���ۿ��� �������Ǿ�� �ϴ� ������Ƽ�� ������ ����
    InDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �ﰢ��
}
