#pragma once

//////////////////////////////////////////////////////////////////////////
// Include
//////////////////////////////////////////////////////////////////////////
#include <directxmath.h>
using namespace DirectX;

class CCamera
{
private:

public:
	CCamera() = default;
	CCamera(const CCamera& InOther) = default;
	~CCamera() = default;

public:
	inline void SetPosition(const XMFLOAT3& InPosition) { Position = InPosition; }
	inline void SetRotation(const XMFLOAT3& InRotation) { Rotation = InRotation; }
	inline XMFLOAT3 GetPosition() const { return Position; }
	inline XMFLOAT3 GetRotation() const { return Rotation; }
	inline void GetViewMatrix(XMMATRIX& OutViewMatrix) const { OutViewMatrix = ViewMatrix; }

public:
	/** ī�޶��� ��ġ�� ȸ���� ������� �� ��Ʈ������ �����ϴ� �� ��� */
	void Render();
	
private:
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMMATRIX ViewMatrix;
};