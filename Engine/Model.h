#pragma once

//////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

// 3D ���� ������ ĸ��ȭ�ϴ� ����
class CModel
{
private:
	struct TVertexType
	{
		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

public:
	CModel() = default;
	CModel(const CModel& InOther) = default;
	~CModel() = default;

public:
	bool Initialize(ID3D11Device* InDevice);
	void Shutdown();
	void Render(ID3D11DeviceContext* InDeviceContext);

public:
	inline int GetIndexCount() const { return IndexCount; }

private:
	/** ���� �� �ε��� ���� ���� ó�� */
	bool InitializeBuffers(ID3D11Device* InDevice);

	void ShutdownBuffers();

	/**
	 * GPU�� �Է� ��������� ���� ���ۿ� �ε��� ���۸� Ȱ������ �����ϵ��� ��
	 *	-> GPU�� Ȱ�� ���ؽ� ���۰� ������ ���̴��� ����Ͽ� �ش� ���� ������ ����
	 */
	void RenderBuffers(ID3D11DeviceContext* InDeviceContext);

private:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	int VertexCount;
	int IndexCount;
};