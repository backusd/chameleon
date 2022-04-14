#include "CenterOnOriginMoveLookController.h"

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;


CenterOnOriginMoveLookController::CenterOnOriginMoveLookController(HWND hWnd) :
    MoveLookController(hWnd)
{
    ResetState();

    m_eyeVec = { 0.0f, 0.0f, 10.0f, 0.0f };     // Start at 10 in the positive Z direction
    m_atVec = { 0.0f, 0.0f, 0.0f, 0.0f };		// look at the origin
    m_upVec = { 0.0f, 1.0f, 0.0f, 0.0f };		// Up in the positive y-direction

    SetupImGui();
}

void CenterOnOriginMoveLookController::SetupImGui()
{
    m_cameraPosition = XMFLOAT3(0.0f, 0.0f, 10.0f);
    m_cameraLookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_cameraUpDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);

    m_cameraPositionMax = XMFLOAT3(100.0f, 100.0f, 100.0f);
    m_cameraPositionMin = XMFLOAT3(-100.0f, -100.0f, -100.0f);

    m_cameraLookAtMax = XMFLOAT3(100.0f, 100.0f, 100.0f);
    m_cameraLookAtMin = XMFLOAT3(-100.0f, -100.0f, -100.0f);

    m_cameraUpDirectionMax = XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_cameraUpDirectionMin = XMFLOAT3(-1.0f, -1.0f, -1.0f);
}

void CenterOnOriginMoveLookController::ResetState()
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

    m_movingToNewLocation = false;
    m_moveCompleted = false;
    m_moveStartTime = 0.0;
    m_movementMaxTime = 0.0;
    m_timeAtLastMoveUpdate = 0.0;
    m_totalRotationAngle = 0.0f;
    m_rotatingLeftRight = false;
    m_rotatingUpDown = false;

    m_elapsedTime = 0.0;

    m_eyeTarget  = XMFLOAT3(0.0f, 0.0f, 10.0f);
    m_eyeInitial = XMFLOAT3(0.0f, 0.0f, 10.0f);
    m_upTarget   = XMFLOAT3(0.0f, 1.0f, 0.0f);
    m_upInitial  = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

void CenterOnOriginMoveLookController::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
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


void CenterOnOriginMoveLookController::InitializeAutomatedMove(double maxMoveTime)
{
    // Set m_movingToNewLocation = true so the SceneRenderer knows to update the view matrix
    m_movingToNewLocation = true;

    // Set the move completed flag to false
    m_moveCompleted = false;

    // Reset the start time to -1 to signal it needs to be set in the next Update
    m_moveStartTime = -1.0;

    // Set the movement max time to 0.1 seconds, so the zoom completes in that time
    m_movementMaxTime = maxMoveTime;

    DirectX::XMStoreFloat3(&m_eyeInitial, m_eyeVec);
    DirectX::XMStoreFloat3(&m_upInitial, m_upVec);
}

void CenterOnOriginMoveLookController::ZoomIn(int mouseX, int mouseY)
{
    // Only update if not already moving (this avoids a flood of WM_MOUSEWHEEL messages)
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.1 seconds for the move
        InitializeAutomatedMove(0.1);

        // Set eye target location to be 10% closer than the current location for every wheel delta of -120
        float factor = 0.9f;
        m_eyeTarget.x = m_eyeInitial.x * factor;
        m_eyeTarget.y = m_eyeInitial.y * factor;
        m_eyeTarget.z = m_eyeInitial.z * factor;

        m_upTarget = m_upInitial;
    }
}
void CenterOnOriginMoveLookController::ZoomOut(int mouseX, int mouseY)
{
    // Only update if not already moving (this avoids a flood of WM_MOUSEWHEEL messages)
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.1 seconds for the move
        InitializeAutomatedMove(0.1);

        // Set eye target location to be 10% further than the current location for every wheel delta of -120
        float factor = 1.1f;
        m_eyeTarget.x = m_eyeInitial.x * factor;
        m_eyeTarget.y = m_eyeInitial.y * factor;
        m_eyeTarget.z = m_eyeInitial.z * factor;

        m_upTarget = m_upInitial;
    }
}

void CenterOnOriginMoveLookController::MouseMove()
{
    if (m_LButtonDown)
    {
        RECT rect;
        GetClientRect(m_hWnd, &rect);

        // Compute the eye distance to center
        float radius = 0.0f;
        DirectX::XMStoreFloat(&radius, DirectX::XMVector3Length(m_eyeVec));

        // If the pointer were to move from the middle of the screen to the far right,
        // that should produce one full rotation. Therefore, set a rotationFactor = 2
        float rotationFactor = 2.0f;
        float width = static_cast<float>(rect.right - rect.left);
        float height = static_cast<float>(rect.bottom - rect.top);

        float radiansPerPixelX = (DirectX::XM_2PI / width) * rotationFactor;
        float radiansPerPixelY = (DirectX::XM_2PI / height) * rotationFactor;

        float thetaX = radiansPerPixelX * (m_mouseDownInitialPositionX - m_mouseCurrentPositionX);
        float thetaY = radiansPerPixelY * (m_mouseDownInitialPositionY - m_mouseCurrentPositionY);


        // Rotate
        RotateLeftRight(thetaX);
        RotateUpDown(-thetaY);

        // reset the mouse position variables
        m_mouseDownInitialPositionX = m_mouseCurrentPositionX;
        m_mouseDownInitialPositionY = m_mouseCurrentPositionY;
    }
}

void CenterOnOriginMoveLookController::RotateLeftRight(float theta)
{
    // Use Rodrigue's Rotation Formula
        //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //     v_rot : vector after rotation
        //     v     : vector before rotation
        //     theta : angle of rotation
        //     k     : unit vector representing axis of rotation
        //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))

    XMVECTOR v = m_eyeVec;
    XMVECTOR k = m_upVec;
    m_eyeVec = v * cos(theta) + XMVector3Cross(k, v) * sin(theta) + k * XMVector3Dot(k, v) * (1 - cos(theta));

    // Do NOT change the up-vector
}
void CenterOnOriginMoveLookController::RotateUpDown(float theta)
{
    // Use Rodrigue's Rotation Formula
    //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    //     v_rot : vector after rotation
    //     v     : vector before rotation
    //     theta : angle of rotation
    //     k     : unit vector representing axis of rotation
    //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))


    // The axis of rotation vector for up/down rotation will be the cross product 
    // between the eye-vector and the up-vector (must make it a unit vector)
    XMVECTOR v = m_eyeVec;
    XMVECTOR k = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_eyeVec, m_upVec));
    m_eyeVec = v * cos(theta) + DirectX::XMVector3Cross(k, v) * sin(theta) + k * DirectX::XMVector3Dot(k, v) * (1 - cos(theta));

    // Now update the new up-vector should be the cross product between the k-vector and the new eye-vector
    m_upVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(k, m_eyeVec));
}

void CenterOnOriginMoveLookController::UpdatePosition()
{
    // Loop over the char buffer and process specific keys
    for (char c : m_charBuffer)
    {
        switch (c)
        {
        case 'c': CenterOnFace(); break;
        case 'a': RotateLeft90(); break;
        case 'd': RotateRight90(); break;
        case 'w': RotateUp90(); break;
        case 's': RotateDown90(); break;
        }
    }

    // If an arrow key is being held down, rotate
    if (m_up || m_down || m_left || m_right)
    {
        // Cancel out any existing automated movement
        m_movingToNewLocation = false;

        // When a button is pressed, we must begin tracking the time before we can make an update
        if (m_elapsedTime < 0.01f)
        {
            m_elapsedTime = m_currentTime;
            return;
        }

        // Compute the time delta
        double timeDelta = m_currentTime - m_elapsedTime;
        m_elapsedTime = m_currentTime;

        // Compute the rotation
        float radiansPerSecond = 0.5;
        float thetaUpDown = static_cast<float>(timeDelta * radiansPerSecond);
        float thetaLeftRight = thetaUpDown;
        // If rotating up or right, make the angle negative so the rest of the math is the same
        
        if (m_up)
            thetaUpDown *= -1;

        if (m_right)
            thetaLeftRight *= -1;

        if (m_up || m_down)
            RotateUpDown(thetaUpDown);

        if (m_left || m_right)
            RotateLeftRight(thetaLeftRight);
    }
    else
    {
        m_elapsedTime = 0.0;
    }

    // Perform any automated movements
    if (m_movingToNewLocation)
    {
        // if the move has been completed, then done
        if (m_moveCompleted)
        {
            m_movingToNewLocation = false;
            m_rotatingLeftRight = false;
            m_rotatingUpDown = false;
        }
        else
        {
            // If the move start time is less than 0, it needs to be set
            if (m_moveStartTime < 0.0)
            {
                m_moveStartTime = m_currentTime;
                m_timeAtLastMoveUpdate = m_moveStartTime;
            }

            // If rotating left/right, just compute the necessary angle and call RotateLeftRight / RotateUpDown
            if (m_rotatingLeftRight || m_rotatingUpDown)
            {
                double currentTime = m_currentTime;
                double timeDelta;
                if (m_moveStartTime + m_movementMaxTime < currentTime)
                {
                    m_moveCompleted = true;
                    timeDelta = m_moveStartTime + m_movementMaxTime - m_timeAtLastMoveUpdate;
                }
                else
                    timeDelta = currentTime - m_timeAtLastMoveUpdate;

                float theta = m_totalRotationAngle * static_cast<float>(timeDelta / m_movementMaxTime);

                if (m_rotatingLeftRight)
                    RotateLeftRight(theta);
                else
                    RotateUpDown(theta);

                m_timeAtLastMoveUpdate = currentTime;
            }
            else
            {
                // Compute the ratio of elapsed time / allowed time to complete
                double timeRatio = (m_currentTime - m_moveStartTime) / m_movementMaxTime;

                // if the current time is passed the max time, just assign final postion
                // Need to also set the updated view matrix has been read flag because SceneRenderer
                // will read the view matrix on the next Update call. Once that is done, we can set
                // movingToNewLocation to false (above)
                if (timeRatio >= 1.0)
                {
                    m_moveCompleted = true;
                    m_eyeVec = DirectX::XMLoadFloat3(&m_eyeTarget);
                    m_upVec = DirectX::XMLoadFloat3(&m_upTarget);
                }
                else
                {
                    // Compute the intermediate position
                    XMFLOAT3 eyeCurrent;
                    eyeCurrent.x = m_eyeInitial.x + static_cast<float>((static_cast<double>(m_eyeTarget.x) - m_eyeInitial.x) * timeRatio);
                    eyeCurrent.y = m_eyeInitial.y + static_cast<float>((static_cast<double>(m_eyeTarget.y) - m_eyeInitial.y) * timeRatio);
                    eyeCurrent.z = m_eyeInitial.z + static_cast<float>((static_cast<double>(m_eyeTarget.z) - m_eyeInitial.z) * timeRatio);

                    m_eyeVec = DirectX::XMLoadFloat3(&eyeCurrent);

                    // Compute the intermediate position
                    XMFLOAT3 upCurrent;
                    upCurrent.x = m_upInitial.x + static_cast<float>((static_cast<double>(m_upTarget.x) - m_upInitial.x) * timeRatio);
                    upCurrent.y = m_upInitial.y + static_cast<float>((static_cast<double>(m_upTarget.y) - m_upInitial.y) * timeRatio);
                    upCurrent.z = m_upInitial.z + static_cast<float>((static_cast<double>(m_upTarget.z) - m_upInitial.z) * timeRatio);

                    m_upVec = DirectX::XMLoadFloat3(&upCurrent);
                }
            }
        }
    }
}

bool CenterOnOriginMoveLookController::IsMoving()
{
    return m_up || m_down || m_left || m_right || m_LButtonDown || m_movingToNewLocation;
}

/*
void CenterOnOriginMoveLookController::OnLButtonDoubleClick()
{
    // Set automated move flags and initial data - 0.5 seconds for the move
    InitializeAutomatedMove(0.5);

    // Set eye target location to half the distance to the center
    m_eyeTarget.x = m_eyeInitial.x / 2.0f;
    m_eyeTarget.y = m_eyeInitial.y / 2.0f;
    m_eyeTarget.z = m_eyeInitial.z / 2.0f;

    m_upTarget = m_upInitial;
}
*/

void CenterOnOriginMoveLookController::CenterOnFace()
{
    // Only allow a single left/right movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);

        // Determine the coordinate with the max value and 0 out the other ones
        m_eyeTarget.x = m_eyeInitial.x;
        m_eyeTarget.y = m_eyeInitial.y;
        m_eyeTarget.z = m_eyeInitial.z;

        XMFLOAT3 length3;
        DirectX::XMStoreFloat3(&length3, DirectX::XMVector3Length(m_eyeVec));
        float length = length3.x;

        m_eyeTarget.x = (std::abs(m_eyeInitial.x) < std::abs(m_eyeInitial.y) || std::abs(m_eyeInitial.x) < std::abs(m_eyeInitial.z)) ? 0.0f : length;
        m_eyeTarget.y = (std::abs(m_eyeInitial.y) < std::abs(m_eyeInitial.x) || std::abs(m_eyeInitial.y) < std::abs(m_eyeInitial.z)) ? 0.0f : length;
        m_eyeTarget.z = (std::abs(m_eyeInitial.z) < std::abs(m_eyeInitial.x) || std::abs(m_eyeInitial.z) < std::abs(m_eyeInitial.y)) ? 0.0f : length;

        m_eyeTarget.x *= (m_eyeInitial.x < 0.0f) ? -1.0f : 1.0f;
        m_eyeTarget.y *= (m_eyeInitial.y < 0.0f) ? -1.0f : 1.0f;
        m_eyeTarget.z *= (m_eyeInitial.z < 0.0f) ? -1.0f : 1.0f;



        // Determine the coordinate with the max value and 0 out the other ones
        // Whichever coordinate for the eye target is used must not be used for the up target, so zero it out
        float xInit = (m_eyeTarget.x == 0.0f) ? m_upInitial.x : 0.0f;
        float yInit = (m_eyeTarget.y == 0.0f) ? m_upInitial.y : 0.0f;
        float zInit = (m_eyeTarget.z == 0.0f) ? m_upInitial.z : 0.0f;

        DirectX::XMStoreFloat3(&length3, DirectX::XMVector3Length(m_upVec));
        length = length3.x;

        m_upTarget.x = (std::abs(xInit) < std::abs(yInit) || std::abs(xInit) < std::abs(zInit)) ? 0.0f : length;
        m_upTarget.y = (std::abs(yInit) < std::abs(xInit) || std::abs(yInit) < std::abs(zInit)) ? 0.0f : length;
        m_upTarget.z = (std::abs(zInit) < std::abs(xInit) || std::abs(zInit) < std::abs(yInit)) ? 0.0f : length;

        m_upTarget.x *= (xInit < 0.0f) ? -1.0f : 1.0f;
        m_upTarget.y *= (yInit < 0.0f) ? -1.0f : 1.0f;
        m_upTarget.z *= (zInit < 0.0f) ? -1.0f : 1.0f;
    }
}
void CenterOnOriginMoveLookController::RotateLeft90()
{
    // Only allow a single left/right movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingLeftRight = true;
        m_totalRotationAngle = -1.0f * DirectX::XM_PIDIV2;
    }
}
void CenterOnOriginMoveLookController::RotateRight90()
{
    // Only allow a single left/right movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingLeftRight = true;
        m_totalRotationAngle = DirectX::XM_PIDIV2;
    }
}
void CenterOnOriginMoveLookController::RotateUp90()
{
    // Only allow a single up/down movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingUpDown = true;
        m_totalRotationAngle = DirectX::XM_PIDIV2;
    }
}
void CenterOnOriginMoveLookController::RotateDown90()
{
    // Only allow a single up/down movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingUpDown = true;
        m_totalRotationAngle = -1.0f * DirectX::XM_PIDIV2;
    }
}



void CenterOnOriginMoveLookController::DrawImGui()
{
    // Draw a camera control
    ImGui::Begin("Center On Origin Camera");

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

void CenterOnOriginMoveLookController::LoadImGuiValues()
{
    m_eyeVec = DirectX::XMVectorSet(m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z, 1.0f);
    m_atVec = DirectX::XMVectorSet(m_cameraLookAt.x, m_cameraLookAt.y, m_cameraLookAt.z, 1.0f);
    m_upVec = DirectX::XMVectorSet(m_cameraUpDirection.x, m_cameraUpDirection.y, m_cameraUpDirection.z, 1.0f);
}

void CenterOnOriginMoveLookController::UpdateImGuiValues()
{
    DirectX::XMStoreFloat3(&m_cameraPosition, m_eyeVec);
    DirectX::XMStoreFloat3(&m_cameraLookAt, m_atVec);
    DirectX::XMStoreFloat3(&m_cameraUpDirection, m_upVec);
}