#include "Camera.h"

inline float DEGREE_TO_RADIAN(const float InDegree)
{
	return (InDegree * 0.0174532925f/*pi/180*/);
}

void CCamera::Render()
{
	const XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 lookAt(0.0f, 0.0f, 1.0f);
	const XMFLOAT3 position(Position);

	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);
	XMVECTOR positionVector = XMLoadFloat3(&position);

	// Pith, Yaw, Roll 회전을 라디안 단위로 설정
	const float pitch = DEGREE_TO_RADIAN(Rotation.x);
	const float yaw = DEGREE_TO_RADIAN(Rotation.y);
	const float roll = DEGREE_TO_RADIAN(Rotation.z);

	// 회전 행렬 
	const XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 뷰가 원점에서 올바르게 회전하도록 회전 행렬로 lookAt 및 up 벡터 변환
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotMatrix);
	upVector = XMVector3TransformCoord(upVector, rotMatrix);

	// 회전된 카메라 위치를 보는 사람의 위치로 변환
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 뷰 행렬 세팅
	ViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}