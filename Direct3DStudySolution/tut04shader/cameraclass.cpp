////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 벡터를 위쪽 방향으로 설정한다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// up 값을 XMVECTOR 구조체에 로드한다.
	upVector = XMLoadFloat3(&up);

	// 월드 내의 카메라의 위치를 설정한다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// position 값을 XMVECTOR 구조체에 로드한다.
	positionVector = XMLoadFloat3(&position);

	// 카메라가 바라보는 곳을 기본 값으로 설정한다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// position 값을 XMVECTOR 구조체에 로드한다.
	lookAtVector = XMLoadFloat3(&lookAt);

	// yaw (Y축), pitch (X축), roll (Z축) 회전을 라디안으로 설정한다.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// yaw, pitch, roll로부터 회전 행렬을 생성한다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt과 up 벡터를 회전 행렬을 통해 평행이동시켜서 뷰 행렬을 원점에서 올바르게 회전시킨다.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전된 카메라 위치를 관찰자 위치로 이동시킨다.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 최종적으로 세 개의 업데이트된 벡터로부터 뷰 행렬을 생성한다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}