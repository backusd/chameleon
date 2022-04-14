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
    m_cameraPosition = XMFLOAT3(115.0f, 71.0f, 230.0f);
    m_cameraLookAt = XMFLOAT3(150.0f, 70.0f, 270.0f);
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

void FlyMoveLookController::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
    // At the beginning of the update, load the eye/at/up values for the selected camera mode as they may
    // have changed because of the menu sliders
    LoadImGuiValues();

    m_currentTime = timer->GetTotalSeconds();

    Mouse::Event e;
    std::ostringstream oss;

    Keyboard::Event keyEvent;

    // Process mouse events
    while (!mouse->IsEmpty())
    {
        e = mouse->Read();
        switch (e.GetType())
        {
        case Mouse::Event::Type::WheelUp: ZoomIn(mouse->GetPosX(), mouse->GetPosY()); break;
        case Mouse::Event::Type::WheelDown: ZoomOut(mouse->GetPosX(), mouse->GetPosY()); break;
        case Mouse::Event::Type::LPress:
            // Only register the LPress if the RButton and MButton are not already down
            if (!(m_RButtonDown || m_MButtonDown))
            {
                m_LButtonDown = true;
                m_mouseDownInitialPositionX = m_mouseCurrentPositionX = static_cast<float>(mouse->GetPosX());
                m_mouseDownInitialPositionY = m_mouseCurrentPositionY = static_cast<float>(mouse->GetPosY());
            }
            break;

        case Mouse::Event::Type::LRelease: m_LButtonDown = false; break;
        case Mouse::Event::Type::RPress:
            // Only register the RPress if the LButton and MButton are not already down
            if (!(m_LButtonDown || m_MButtonDown))
            {
                m_RButtonDown = true;
                m_mouseDownInitialPositionX = m_mouseCurrentPositionX = static_cast<float>(mouse->GetPosX());
                m_mouseDownInitialPositionY = m_mouseCurrentPositionY = static_cast<float>(mouse->GetPosY());
            }
            break;

        case Mouse::Event::Type::RRelease: m_RButtonDown = false; break;
        case Mouse::Event::Type::MPress:
            // Only register the MPress if the LButton and RButton are not already down
            if (!(m_LButtonDown || m_RButtonDown))
            {
                m_MButtonDown = true;
                m_mouseDownInitialPositionX = m_mouseCurrentPositionX = static_cast<float>(mouse->GetPosX());
                m_mouseDownInitialPositionY = m_mouseCurrentPositionY = static_cast<float>(mouse->GetPosY());
            }
            break;

        case Mouse::Event::Type::MRelease: m_MButtonDown = false; break;
        case Mouse::Event::Type::Move:
            m_mouseCurrentPositionX = static_cast<float>(mouse->GetPosX());
            m_mouseCurrentPositionY = static_cast<float>(mouse->GetPosY());
            MouseMove();
            break;
        default:
            break;
        }

    }

    // Process keyboard events, but only if a mouse button is not down
    if (m_LButtonDown || m_MButtonDown || m_RButtonDown)
    {
        // Just drop any key events if mouse is down
        keyboard->FlushKey();
    }
    else
    {
        while (!keyboard->KeyIsEmpty())
        {
            keyEvent = keyboard->ReadKey();
            switch (keyEvent.GetCode())
            {
            case VK_SHIFT:      m_shift = keyEvent.IsPress(); break;
            case VK_CONTROL:    m_ctrl = keyEvent.IsPress(); break;
            case VK_MENU:       m_alt = keyEvent.IsPress(); break;    // ALT key
            case VK_LEFT:       m_left = keyEvent.IsPress(); break;
            case VK_UP:         m_up = keyEvent.IsPress(); break;
            case VK_RIGHT:      m_right = keyEvent.IsPress(); break;
            case VK_DOWN:       m_down = keyEvent.IsPress(); break;
            }
        }

        // Read in each char into a vector that will then get used in the UpdatePosition function
        while (!keyboard->CharIsEmpty())
        {
            m_charBuffer.push_back(keyboard->ReadChar());
        }
    }

    // Call update position to check if any of the new variables have been set and update the position accordingly
    UpdatePosition();

    // Clear the char buffer as UpdatePosition should now be done with them
    m_charBuffer.clear();


    m_previousTime = m_currentTime;


    // Update the ImGui values with the new eye/at/up vec values
    UpdateImGuiValues();
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
    XMVECTOR positionChange = DirectX::XMVectorScale(diffNormal, static_cast<float>(m_moveSpeed * timeDelta));

    // Add this value to the eyeVec and atVec
    m_eyeVec = DirectX::XMVectorAdd(m_eyeVec, positionChange);
    m_atVec = DirectX::XMVectorAdd(m_atVec, positionChange);
}

void FlyMoveLookController::DrawImGui()
{
    // Draw a camera control
    ImGui::Begin("Fly Camera");

    ImGui::Text("Position:");
    ImGui::Text("    X: "); ImGui::SameLine(); ImGui::SliderFloat("##cameraPositionX", &m_cameraPosition.x, m_cameraPositionMin.x, m_cameraPositionMax.x, "%.3f");
    ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::SliderFloat("##cameraPositionY", &m_cameraPosition.y, m_cameraPositionMin.y, m_cameraPositionMax.y, "%.3f");
    ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::SliderFloat("##cameraPositionZ", &m_cameraPosition.z, m_cameraPositionMin.z, m_cameraPositionMax.z, "%.3f");
    ImGui::Text("Look At:");
    ImGui::Text("    X: "); ImGui::SameLine(); ImGui::SliderFloat("##lookAtX", &m_cameraLookAt.x, m_cameraLookAtMin.x, m_cameraLookAtMax.x, "%.3f");
    ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::SliderFloat("##lookAtY", &m_cameraLookAt.y, m_cameraLookAtMin.y, m_cameraLookAtMax.y, "%.3f");
    ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::SliderFloat("##lookAtZ", &m_cameraLookAt.z, m_cameraLookAtMin.z, m_cameraLookAtMax.z, "%.3f");
    ImGui::Text("Up Direction:");
    ImGui::Text("    X: "); ImGui::SameLine(); ImGui::SliderFloat("##upDirectionX", &m_cameraUpDirection.x, m_cameraUpDirectionMin.x, m_cameraUpDirectionMax.x, "%.3f");
    ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::SliderFloat("##upDirectionY", &m_cameraUpDirection.y, m_cameraUpDirectionMin.y, m_cameraUpDirectionMax.y, "%.3f");
    ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::SliderFloat("##upDirectionZ", &m_cameraUpDirection.z, m_cameraUpDirectionMin.z, m_cameraUpDirectionMax.z, "%.3f");

    ImGui::End();
}

void FlyMoveLookController::LoadImGuiValues()
{
    m_eyeVec = DirectX::XMVectorSet(m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z, 1.0f);
    m_atVec = DirectX::XMVectorSet(m_cameraLookAt.x, m_cameraLookAt.y, m_cameraLookAt.z, 1.0f);
    m_upVec = DirectX::XMVectorSet(m_cameraUpDirection.x, m_cameraUpDirection.y, m_cameraUpDirection.z, 1.0f);
}

void FlyMoveLookController::UpdateImGuiValues()
{
    DirectX::XMStoreFloat3(&m_cameraPosition, m_eyeVec);
    DirectX::XMStoreFloat3(&m_cameraLookAt, m_atVec);
    DirectX::XMStoreFloat3(&m_cameraUpDirection, m_upVec);
}