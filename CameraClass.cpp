#include "CameraClass.h"

using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

CameraClass::CameraClass() : 
	m_positionX(0.0f),
	m_positionY(0.0f),
	m_positionZ(0.0f),
	m_rotationX(0.0f),
	m_rotationY(0.0f),
	m_rotationZ(0.0f),
	m_viewMatrix(DirectX::XMMatrixIdentity()),
	m_baseViewMatrix(DirectX::XMMatrixIdentity()),
	m_reflectionViewMatrix(DirectX::XMMatrixIdentity())
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
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
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
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	XMVECTOR lookAtVector = DirectX::XMLoadFloat3(&lookAt);
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	
	XMVECTOR upVector = DirectX::XMLoadFloat3(&up);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);
	lookAtVector = DirectX::XMVectorAdd(lookAtVector, positionVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GenerateBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	XMVECTOR lookAtVector = DirectX::XMLoadFloat3(&lookAt);
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);

	XMVECTOR upVector = DirectX::XMLoadFloat3(&up);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);
	lookAtVector = DirectX::XMVectorAdd(lookAtVector, positionVector);

	// Finally create the view matrix from the three updated vectors.
	m_baseViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void CameraClass::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.  For planar reflection invert the Y position of the camera.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.  Invert the X rotation for reflection.
	pitch = -m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;


	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	XMVECTOR lookAtVector = DirectX::XMLoadFloat3(&lookAt);
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);

	XMVECTOR upVector = DirectX::XMLoadFloat3(&up);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);
	lookAtVector = DirectX::XMVectorAdd(lookAtVector, positionVector);

	// Finally create the view matrix from the three updated vectors.
	m_reflectionViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}