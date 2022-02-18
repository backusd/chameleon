#include "FlyMoveLookController.h"

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;


FlyMoveLookController::FlyMoveLookController(HWND hWnd) :
    MoveLookController(hWnd)
{
    ResetState();

    m_eyeVec = { 0.0f, 0.0f, 2.0f, 0.0f };      // Start at 2 in the positive Z direction
    m_atVec = { 0.0f, 0.0f, 0.0f, 0.0f };		// look at the origin
    m_upVec = { 0.0f, 1.0f, 0.0f, 0.0f };		// Up in the positive y-direction

    SetupImGui();
}

void FlyMoveLookController::SetupImGui()
{
    m_cameraPosition = XMFLOAT3(50.0f, 10.0f, -10.0f);
    m_cameraLookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_cameraUpDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);

    m_cameraPositionMax = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
    m_cameraPositionMin = XMFLOAT3(-100.0f, -100.0f, -100.0f);

    m_cameraLookAtMax = XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
    m_cameraLookAtMin = XMFLOAT3(-1000.0f, -1000.0f, -1000.0f);

    m_cameraUpDirectionMax = XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_cameraUpDirectionMin = XMFLOAT3(-1.0f, -1.0f, -1.0f);
}

void FlyMoveLookController::ResetState()
{
    m_LButtonDown = false;
    m_RButtonDown = false;
    m_MButtonDown = false;
    m_mouseDownInitialPositionX = 0.0f;
    m_mouseDownInitialPositionY = 0.0f;
    m_mouseCurrentPositionX = 0.0f;
    m_mouseCurrentPositionY = 0.0f;

    m_left = false;
    m_right = false;
    m_up = false;
    m_down = false;
    m_shift = false;
    m_ctrl = false;
    m_alt = false;

    m_currentTime = 0.0;
    m_previousTime = 0.0;
}

void FlyMoveLookController::UpdatePosition()
{
    if (m_up && !m_down)
    {
        if (m_shift)
            LookUp();
        else
            MoveForward();
    }
    else if (m_down && !m_up)
    {
        if (m_shift)
            LookDown();
        else
            MoveBackward();
    }

    if (m_left && !m_right)
        LookLeft();
    else if (m_right && !m_left)
        LookRight();
}

bool FlyMoveLookController::IsMoving()
{
    return m_up || m_down || m_left || m_right; // || m_mouseDown; // || m_movingToNewLocation;
}

void FlyMoveLookController::LookLeft()
{
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);

    // First compute the difference between the atVec and eyeVec. This gives us a vector
    // that has its tail at the origin, which we can then rotate, and add back to the eyeVec
    // to compute the final atVec
    XMVECTOR diff = DirectX::XMVectorSubtract(m_atVec, m_eyeVec);

    // Rotate the diff about the yAxis
    XMVECTOR yAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
    XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(yAxis, angle);
    XMVECTOR rotatedDiff = DirectX::XMVector3Rotate(diff, rotationQuaternion);

    // Add the diff back to the eye vec to get a slightly different atVec
    m_atVec = DirectX::XMVectorAdd(m_eyeVec, rotatedDiff);
}
void FlyMoveLookController::LookRight()
{
    double timeDelta = m_currentTime - m_previousTime;
    float angle = -1.0f * static_cast<float>(m_turnSpeed * timeDelta);

    // First compute the difference between the atVec and eyeVec. This gives us a vector
    // that has its tail at the origin, which we can then rotate, and add back to the eyeVec
    // to compute the final atVec
    XMVECTOR diff = DirectX::XMVectorSubtract(m_atVec, m_eyeVec);

    // Rotate the diff about the yAxis
    XMVECTOR yAxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
    XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(yAxis, angle);
    XMVECTOR rotatedDiff = DirectX::XMVector3Rotate(diff, rotationQuaternion);

    // Add the diff back to the eye vec to get a slightly different atVec
    m_atVec = DirectX::XMVectorAdd(m_eyeVec, rotatedDiff);
}
void FlyMoveLookController::LookUp()
{
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);

    // If shift is pressed, then we want to pan up
    XMVECTOR diff = DirectX::XMVectorSubtract(m_atVec, m_eyeVec);

    // sideways vector should be the cross product of vector going between atVec and eyeVec (the diff vector) and the upVec
    XMVECTOR sidewaysVector = DirectX::XMVector3Cross(diff, m_upVec);

    // Rotate the diff about the sidewaysVector
    XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(sidewaysVector, angle);
    XMVECTOR rotatedDiff = DirectX::XMVector3Rotate(diff, rotationQuaternion);

    // Add the diff back to the eye vec to get a slightly different atVec
    m_atVec = DirectX::XMVectorAdd(m_eyeVec, rotatedDiff);
}
void FlyMoveLookController::LookDown()
{
    double timeDelta = m_currentTime - m_previousTime;
    float angle = -1.0f * static_cast<float>(m_turnSpeed * timeDelta);

    // If shift is pressed, then we want to pan up
    XMVECTOR diff = DirectX::XMVectorSubtract(m_atVec, m_eyeVec);

    // sideways vector should be the cross product of vector going between atVec and eyeVec (the diff vector) and the upVec
    XMVECTOR sidewaysVector = DirectX::XMVector3Cross(diff, m_upVec);

    // Rotate the diff about the sidewaysVector
    XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationAxis(sidewaysVector, angle);
    XMVECTOR rotatedDiff = DirectX::XMVector3Rotate(diff, rotationQuaternion);

    // Add the diff back to the eye vec to get a slightly different atVec
    m_atVec = DirectX::XMVectorAdd(m_eyeVec, rotatedDiff);
}
void FlyMoveLookController::MoveForward()
{
    double timeDelta = m_currentTime - m_previousTime;

    // Compute the difference between the eyeVec and atVec - and normalize it
    XMVECTOR diffNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_atVec, m_eyeVec));

    // Scale the diffNormalVector according to the velocity and timedelta to get the change in position
    XMVECTOR positionChange = DirectX::XMVectorScale(diffNormal, static_cast<float>(m_moveSpeed * timeDelta));

    // Add this value to the eyeVec and atVec
    m_eyeVec = DirectX::XMVectorAdd(m_eyeVec, positionChange);
    m_atVec = DirectX::XMVectorAdd(m_atVec, positionChange);
}
void FlyMoveLookController::MoveBackward()
{
    double timeDelta = m_currentTime - m_previousTime;

    // Compute the difference between the eyeVec and atVec - and normalize it
    XMVECTOR diffNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_eyeVec, m_atVec));

    // Scale the diffNormalVector according to the velocity and timedelta to get the change in position
    XMVECTOR positionChange = DirectX::XMVectorScale(diffNormal, m_moveSpeed * timeDelta);

    // Add this value to the eyeVec and atVec
    m_eyeVec = DirectX::XMVectorAdd(m_eyeVec, positionChange);
    m_atVec = DirectX::XMVectorAdd(m_atVec, positionChange);
}
