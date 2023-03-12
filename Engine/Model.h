#pragma once

//////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

// 3D 모델의 형상을 캡슐화하는 역할
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
	/** 정점 및 인덱스 버퍼 생성 처리 */
	bool InitializeBuffers(ID3D11Device* InDevice);

	void ShutdownBuffers();

	/**
	 * GPU의 입력 어셈블러에서 정점 버퍼와 인덱스 버퍼를 활성으로 설정하도록 함
	 *	-> GPU에 활성 버텍스 버퍼가 있으면 셰이더를 사용하여 해당 버퍼 렌더링 가능
	 */
	void RenderBuffers(ID3D11DeviceContext* InDeviceContext);

private:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	int VertexCount;
	int IndexCount;
};