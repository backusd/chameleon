#include "MoveLookController.h"

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;


MoveLookController::MoveLookController() :
    m_moveSpeed(10.0),
    m_turnSpeed(0.5)
    //m_elapsedTime(0.0),
    //m_moveStartTime(0.0),
    //m_movementMaxTime(1.0),
    //m_timeAtLastMoveUpdate(0.0),
    //m_totalRotationAngle(0.0f)
{
    ResetState();

    m_eyeVec = { 0.0f, 0.0f, -2.0f, 0.0f };     // Start at 2 in the negative Z direction
    m_atVec = { 0.0f, 0.0f, 0.0f, 0.0f };		// look at the origin
    m_upVec = { 0.0f, 1.0f, 0.0f, 0.0f };		// Up in the positive y-direction
}

void MoveLookController::ResetState()
{
    m_mouseDown = false;
    m_left = false;
    m_right = false;
    m_up = false;
    m_down = false;
    m_shift = false;
    m_ctrl = false;
    m_alt = false;

    m_currentTime = 0.0;
    m_previousTime = 0.0;
    
    //m_movingToNewLocation = false;
    //m_updatedViewMatrixHasBeenRead = false;
    //m_rotatingLeftRight = false;
    //m_rotatingUpDown = false;
}

DirectX::XMMATRIX MoveLookController::ViewMatrix()
{
    return DirectX::XMMatrixLookAtRH(m_eyeVec, m_atVec, m_upVec);
}

void MoveLookController::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse, HWND hWnd)
{
    m_currentTime = timer->GetTotalSeconds();

    Mouse::Event e;
    float factor;
    XMFLOAT3 newEye;
    std::ostringstream oss;

    Keyboard::Event keyEvent;

    // Process keyboard events, but only if the LButton is NOT down
    if (m_mouseDown)
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
    }

    // Call update position to check if any of the new variables have been set and update the position accordingly
    UpdatePosition();

    
    while (!mouse->IsEmpty())
    {
        e = mouse->Read();
        switch (e.GetType())
        {
        case Mouse::Event::Type::WheelUp:
            /*
            factor = 0.05f;
            DirectX::XMStoreFloat3(&newEye, m_eyeVec);
            // newEye.x += factor;
            // newEye.y += factor;
            newEye.z += factor;
            m_eyeVec = DirectX::XMLoadFloat3(&newEye);

            oss << "EYE: (" << newEye.x << ", " << newEye.y << ", " << newEye.z << ")";
            SetWindowText(hWnd, oss.str().c_str());
            */
            break;

        case Mouse::Event::Type::WheelDown:
            /*
            factor = 0.05f;
            DirectX::XMStoreFloat3(&newEye, m_eyeVec);
            //newEye.x -= factor;
            //newEye.y -= factor;
            newEye.z -= factor;
            m_eyeVec = DirectX::XMLoadFloat3(&newEye);

            oss << "EYE: (" << newEye.x << ", " << newEye.y << ", " << newEye.z << ")";
            SetWindowText(hWnd, oss.str().c_str());
            */
            break;

        case Mouse::Event::Type::LPress:
            /*
            m_mouseDown = true;
            m_mousePositionX = m_mousePositionXNew = mouse->GetPosX();
            m_mousePositionY = m_mousePositionYNew = mouse->GetPosY();
            */
            break;

        case Mouse::Event::Type::LRelease:
            /*
            m_mouseDown = false;
            */
            break;

        case Mouse::Event::Type::Move:
            /*
            if (m_mouseDown)
            {
                RECT rect;
                GetClientRect(hWnd, &rect);

                m_mousePositionXNew = mouse->GetPosX();
                m_mousePositionYNew = mouse->GetPosY();

                // Compute the eye distance to center
                float radius = 0.0f;
                DirectX::XMStoreFloat(&radius, DirectX::XMVector3Length(m_eyeVec));

                // If the pointer were to move from the middle of the screen to the far right,
                // that should produce one full rotation. Therefore, set a rotationFactor = 2
                float rotationFactor = 2.0f;
                float width = rect.right - rect.left;
                float height = rect.bottom - rect.top;

                float radiansPerPixelX = (DirectX::XM_2PI / width) * rotationFactor;
                float radiansPerPixelY = (DirectX::XM_2PI / height) * rotationFactor;

                float thetaX = radiansPerPixelX * (m_mousePositionX - m_mousePositionXNew);
                float thetaY = radiansPerPixelY * (m_mousePositionY - m_mousePositionYNew);


                // Rotate
                RotateLeftRight(thetaX);
                RotateUpDown(-thetaY);

                // reset the mouse position variables
                m_mousePositionX = m_mousePositionXNew;
                m_mousePositionY = m_mousePositionYNew;
            }
            */
            break;

        default:
            break;
        }

    }

    m_previousTime = m_currentTime;

    /*
    if (m_mouseDown)
    {
        // Cancel out any existing automated movement
        m_movingToNewLocation = false;

        // Compute the eye distance to center
        float radius = 0.0f;
        DirectX::XMStoreFloat(&radius, DirectX::XMVector3Length(m_eyeVec));

        // If the pointer were to move from the middle of the screen to the far right,
        // that should produce one full rotation. Therefore, set a rotationFactor = 2
        float rotationFactor = 2.0f;
        float renderPaneWidth = renderPaneRect.right - renderPaneRect.left;
        float renderPaneHeight = renderPaneRect.bottom - renderPaneRect.top;

        float radiansPerPixelX = (DirectX::XM_2PI / renderPaneWidth) * rotationFactor;
        float radiansPerPixelY = (DirectX::XM_2PI / renderPaneHeight) * rotationFactor;

        float thetaX = radiansPerPixelX * (m_mousePositionX - m_mousePositionXNew);
        float thetaY = radiansPerPixelY * (m_mousePositionYNew - m_mousePositionY);

        // Rotate
        RotateLeftRight(thetaX);
        RotateUpDown(thetaY);

        // reset the mouse position variables
        m_mousePositionX = m_mousePositionXNew;
        m_mousePositionY = m_mousePositionYNew;
    }
    else if (m_up || m_down || m_left || m_right)
    {
        // Cancel out any existing automated movement
        m_movingToNewLocation = false;


        // When a button is pressed, we must begin tracking the time before we can make an update
        if (m_elapsedTime < 0.01f)
        {
            m_elapsedTime = timer.GetTotalSeconds();
            return;
        }

        // Compute the time delta
        double currentTime = timer.GetTotalSeconds();
        double timeDelta = currentTime - m_elapsedTime;
        m_elapsedTime = currentTime;

        // Compute the rotation
        float radiansPerSecond = 0.5;
        float theta = static_cast<float>(timeDelta * radiansPerSecond);

        // If rotating up or right, make the angle negative so the rest of the math is the same
        if (m_up || m_right)
            theta *= -1;

        if (m_up || m_down)
            RotateUpDown(theta);

        if (m_left || m_right)
            RotateLeftRight(theta);
    }
    else if (m_movingToNewLocation)
    {
        // if the view matrix has officially been read by SceneRenderer, no need to perform any update here
        if (m_updatedViewMatrixHasBeenRead)
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
                m_moveStartTime = timer.GetTotalSeconds();
                m_timeAtLastMoveUpdate = m_moveStartTime;
            }

            // If rotating left/right, just compute the necessary angle and call RotateLeftRight / RotateUpDown
            if (m_rotatingLeftRight || m_rotatingUpDown)
            {
                double currentTime = timer.GetTotalSeconds();
                double timeDelta;
                if (m_moveStartTime + m_movementMaxTime < currentTime)
                {
                    m_updatedViewMatrixHasBeenRead = true;
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
                double timeRatio = (timer.GetTotalSeconds() - m_moveStartTime) / m_movementMaxTime;

                // if the current time is passed the max time, just assign final postion
                // Need to also set the updated view matrix has been read flag because SceneRenderer
                // will read the view matrix on the next Update call. Once that is done, we can set
                // movingToNewLocation to false (above)
                if (timeRatio >= 1.0)
                {
                    m_updatedViewMatrixHasBeenRead = true;
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
    */
}

/*
void MoveLookController::RotateLeftRight(float theta)
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

void MoveLookController::RotateUpDown(float theta)
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
*/

void MoveLookController::UpdatePosition()
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

bool MoveLookController::IsMoving()
{
    return m_up || m_down || m_left || m_right || m_mouseDown; // || m_movingToNewLocation;
}

void MoveLookController::LookLeft()
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
void MoveLookController::LookRight()
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
void MoveLookController::LookUp()
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
void MoveLookController::LookDown()
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
void MoveLookController::MoveForward()
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
void MoveLookController::MoveBackward()
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

/*
void MoveLookController::OnLButtonDown(float mouseX, float mouseY)
{
    // When the pointer is pressed begin tracking the pointer movement.
    m_mouseDown = true;
    m_mousePositionX = m_mousePositionXNew = mouseX;
    m_mousePositionY = m_mousePositionYNew = mouseY;
}

void MoveLookController::OnLButtonUp(float mouseX, float mouseY)
{
    // Stop tracking pointer movement when the pointer is released.
    m_mouseDown = false;
    m_mousePositionX = m_mousePositionXNew = mouseX;
    m_mousePositionY = m_mousePositionYNew = mouseY;
}

void MoveLookController::OnLButtonDoubleClick()
{
    // Set automated move flags and initial data - 0.5 seconds for the move
    InitializeAutomatedMove(0.5);

    // Set eye target location to half the distance to the center
    m_eyeTarget.x = m_eyeInitial.x / 2.0f;
    m_eyeTarget.y = m_eyeInitial.y / 2.0f;
    m_eyeTarget.z = m_eyeInitial.z / 2.0f;

    m_upTarget = m_upInitial;
}

void MoveLookController::OnMouseMove(float mouseX, float mouseY)
{
    m_mousePositionXNew = mouseX;
    m_mousePositionYNew = mouseY;
}
void MoveLookController::OnMouseLeave()
{

}
void MoveLookController::OnMouseWheel(int wheelDelta)
{
    // Only update if not already moving (this avoids a flood of WM_MOUSEWHEEL messages)
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.1 seconds for the move
        InitializeAutomatedMove(0.1);

        // Set eye target location to be 10% closer than the current location for every wheel delta of -120
        // (or further if wheel delta is positive)
        float factor = 1.0f + (0.1f * (wheelDelta / 120.f));
        m_eyeTarget.x = m_eyeInitial.x * factor;
        m_eyeTarget.y = m_eyeInitial.y * factor;
        m_eyeTarget.z = m_eyeInitial.z * factor;

        m_upTarget = m_upInitial;
    }
}

void MoveLookController::CenterOnFace()
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
void MoveLookController::RotateLeft90()
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
void MoveLookController::RotateRight90()
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
void MoveLookController::RotateUp90()
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
void MoveLookController::RotateDown90()
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

void MoveLookController::InitializeAutomatedMove(double maxMoveTime)
{
    // Set m_movingToNewLocation = true so the SceneRenderer knows to update the view matrix
    m_movingToNewLocation = true;

    // Set the move completed flag to false
    m_updatedViewMatrixHasBeenRead = false;

    // Reset the start time to -1 to signal it needs to be set in the next Update
    m_moveStartTime = -1.0;

    // Set the movement max time to 0.1 seconds, so the zoom completes in that time
    m_movementMaxTime = maxMoveTime;

    DirectX::XMStoreFloat3(&m_eyeInitial, m_eyeVec);
    DirectX::XMStoreFloat3(&m_upInitial, m_upVec);
}

void MoveLookController::OnKeyDown(unsigned char keycode)
{
    switch (keycode)
    {
    case VK_UP:    m_up = true; break;
    case VK_DOWN:  m_down = true; break;
    case VK_LEFT:  m_left = true; break;
    case VK_RIGHT: m_right = true; break;
    case VK_SHIFT: m_shift = true; break;
    case VK_CONTROL: m_ctrl = true; break;
    }
}

void MoveLookController::OnKeyUp(unsigned char keycode)
{
    switch (keycode)
    {
    case VK_UP:    m_up = false; break;
    case VK_DOWN:  m_down = false; break;
    case VK_LEFT:  m_left = false; break;
    case VK_RIGHT: m_right = false; break;
    case VK_SHIFT: m_shift = false; break;
    case VK_CONTROL: m_ctrl = false; break;
    }

    // If no longer moving or rotating, reset the time to 0
    if (!(m_up || m_down || m_left || m_right))
        m_elapsedTime = 0.0f;
}
*/