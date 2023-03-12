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

	// Pith, Yaw, Roll ȸ���� ���� ������ ����
	const float pitch = DEGREE_TO_RADIAN(Rotation.x);
	const float yaw = DEGREE_TO_RADIAN(Rotation.y);
	const float roll = DEGREE_TO_RADIAN(Rotation.z);

	// ȸ�� ��� 
	const XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// �䰡 �������� �ùٸ��� ȸ���ϵ��� ȸ�� ��ķ� lookAt �� up ���� ��ȯ
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotMatrix);
	upVector = XMVector3TransformCoord(upVector, rotMatrix);

	// ȸ���� ī�޶� ��ġ�� ���� ����� ��ġ�� ��ȯ
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// �� ��� ����
	ViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}