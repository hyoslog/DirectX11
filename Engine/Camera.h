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
	/** 카메라의 위치와 회전을 기반으로 뷰 매트릭스를 생성하는 데 사용 */
	void Render();
	
private:
	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMMATRIX ViewMatrix;
};