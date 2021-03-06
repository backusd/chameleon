#include "MoveLookController.h"
#include "Player.h"
#include "Terrain.h"

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

using DirectX::operator*;

MoveLookController::MoveLookController(HWND hWnd, std::shared_ptr<DeviceResources> deviceResources) :
    m_hWnd(hWnd),
    m_deviceResources(deviceResources),
    m_moveSpeed(10.0),
    m_turnSpeed(0.5),
    m_player(nullptr),
    m_terrain(nullptr),
    m_r(35.0f),
    m_theta(0.0f),
    m_phi(DirectX::XM_PI / 2.5f) // 2.5 is somewhat random and just gives a pleasing angle for the camera
{
    ResetState();
    UpdateProjectionMatrix();

    // Vectors can have non-sensical values to start because they will be updated
    // correctly once the player is bound (probably want to make sure the math doesn't throw
    // a weird error, so don't just make everything 0)
    m_eyeVec = { 0.0f, 0.0f, -1.0f, 0.0f };
    m_atVec = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_upVec = { 0.0f, 1.0f, 0.0f, 0.0f };
}

void MoveLookController::ResetState()
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
    m_a = false;
    m_w = false;
    m_s = false;
    m_d = false;

    m_currentTime = 0.0;
    m_previousTime = 0.0;

    m_player = nullptr;
    m_terrain = nullptr;
    
    //m_movingToNewLocation = false;
    //m_updatedViewMatrixHasBeenRead = false;
    //m_rotatingLeftRight = false;
    //m_rotatingUpDown = false;
}

void MoveLookController::UpdateProjectionMatrix()
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);

    // Perspective Matrix
    float aspectRatio = static_cast<float>(rect.right - rect.left) / static_cast<float>(rect.bottom - rect.top); // width / height
    float fovAngleY = DirectX::XM_PI / 4;

    // This is a simple example of a change that can be made when the app is in portrait or snapped view
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // Note that the OrientationTransform3D matrix is post-multiplied here
    // in order to correctly orient the scene to match the display orientation.
    // This post-multiplication step is required for any draw calls that are
    // made to the swap chain render target. For draw calls to other targets,
    // this transform should not be applied.

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovRH(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    XMFLOAT4X4 orientation = m_deviceResources->OrientationTransform3D();
    XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    // Projection Matrix (No Transpose)
    m_projectionMatrix = perspectiveMatrix * orientationMatrix;
}

XMMATRIX MoveLookController::ViewMatrix()
{
    return DirectX::XMMatrixLookAtRH(m_eyeVec, m_atVec, m_upVec);
}

void MoveLookController::SetPlayer(std::shared_ptr<Player> player) 
{
    // Set the player and adjust the camera location and direction
    m_player = player;

    // The nanosuit faces the positive z direction but the default camera location 
    // is to face the negative x direction. Rotating the camera negative 90 degrees
    // should line up the camera direction behind the player
    m_theta = -DirectX::XM_PIDIV2;

    UpdateCameraLocation();
}
void MoveLookController::UpdateCameraLocation()
{
    // Camera should point at the center of the player
    XMFLOAT3 center = m_player->CenterOfModel();
    m_atVec = DirectX::XMLoadFloat3(&center);


    // Convert the spherical coordinates for the camera into rectangular coordinates
    // So this is a little bit tricky. We want to swap the z and y coordinates, but we
    // also want to keep working in a right handed coordinate system. The way to set this
    // up is to imagine the axes where x is coming at you, y is going up, and that forces
    // z to go to the left. Then define theta as the angle in the xz-plane starting from
    // the positive x-axis and going toward the positive z-axis. Then define phi as the angle
    // going from the positive y-axis towards the negative y-axis. If you do this, the math
    // works out exactly the same as it would for a normal spherical coordinate system. The
    // BIG CAVEAT is that you have to think of positive theta as going clockwise, instead of 
    // counter-clockwise
    float x = m_r * sin(m_phi) * cos(m_theta);
    float z = m_r * sin(m_phi) * sin(m_theta);
    float y = m_r * cos(m_phi);

    // Compute the location for the camera
    center.x += x;
    center.y += y;
    center.z += z;
    m_eyeVec = DirectX::XMLoadFloat3(&center);
}

void MoveLookController::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
    m_currentTime = timer->GetTotalSeconds();
    m_timeDelta = m_currentTime - m_previousTime;

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

        case Mouse::Event::Type::LRelease: 
            GoToClickLocation(static_cast<float>(mouse->GetPosX()), static_cast<float>(mouse->GetPosY()));
            m_LButtonDown = false; 
            break;

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
            case 'A':           m_a = keyEvent.IsPress(); break;
            case 'W':           m_w = keyEvent.IsPress(); break;
            case 'S':           m_s = keyEvent.IsPress(); break;
            case 'D':           m_d = keyEvent.IsPress(); break;
            }
        }

        // for non-WASD keys, just read from the keyboard's char buffer. The char will be
        // placed on the keyboard's char queue when the key is pressed down, so there is no way
        // of knowing when the char is released. This should be fine for now
        while (!keyboard->CharIsEmpty())
        {
            switch (keyboard->ReadChar())
            {
            case 'c': CenterCameraBehindPlayer(); break;
            }
        }
    }

    // Call update position to check if any of the new variables have been set and update the position accordingly
    UpdatePosition();    

    UpdateCameraLocation();

    m_previousTime = m_currentTime;
}

void MoveLookController::ProcessKeyboardEvents(std::shared_ptr<Keyboard> keyboard)
{
    Keyboard::Event keyEvent;
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
        case 'A':           m_a = keyEvent.IsPress(); break;
        case 'W':           m_w = keyEvent.IsPress(); break;
        case 'S':           m_s = keyEvent.IsPress(); break;
        case 'D':           m_d = keyEvent.IsPress(); break;
        }
    }

    UpdatePosition();
}

void MoveLookController::GoToClickLocation(float x, float y)
{
    // The passed in x/y values are the screen coordinates of the click
    // Convert this to a location and direction vector that can be passed 
    // to the terrain to identify the x/y/z coordinate on the map

    XMVECTOR rayOriginVector, rayDestinationVector, rayDirectionVector;

    D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
    XMMATRIX viewMatrix = ViewMatrix();
    XMFLOAT3 eye;
    DirectX::XMStoreFloat3(&eye, m_eyeVec);

    // Here, we use the identity matrix for the World matrix because we don't want to translate
    // the vectors as if they were at the origin. If we did want to do that, we would use XMMatrixTranslation(eye.x, eye.y, eye.z)
    rayOriginVector = XMVector3Unproject(
        DirectX::XMVectorSet(x, y, 0.0f, 0.0f), // click point near vector
        viewport.TopLeftX,
        viewport.TopLeftY,
        viewport.Width,
        viewport.Height,
        0,
        1,
        m_projectionMatrix,
        viewMatrix,
        DirectX::XMMatrixIdentity());

    rayDestinationVector = XMVector3Unproject(
        DirectX::XMVectorSet(x, y, 1.0f, 0.0f), // click point far vector
        viewport.TopLeftX,
        viewport.TopLeftY,
        viewport.Width,
        viewport.Height,
        0,
        1,
        m_projectionMatrix,
        viewMatrix,
        DirectX::XMMatrixIdentity());

    rayDirectionVector = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestinationVector, rayOriginVector));

    XMFLOAT3 origin, direction;
    XMStoreFloat3(&origin, rayOriginVector);
    XMStoreFloat3(&direction, rayDirectionVector);

    // Get the terrain location of the click (clickLocation is an out param)
    XMFLOAT3 clickLocation;
    if (m_terrain->GetClickLocation(origin, direction, clickLocation))
    {
        // If the click is actually on the map, update the player to move to that location
        float speed = m_ctrl ? 20.0f : 10.0f;
        m_player->MoveTo(clickLocation, speed);
    }
}

void MoveLookController::CenterCameraBehindPlayer()
{
    m_theta = -m_player->Yaw() - DirectX::XM_PIDIV2;
}

void MoveLookController::ZoomIn(int mouseX, int mouseY)
{
    // For the game movelookController, zooming in just requires decreasing m_r
    // Don't allow m_r to be less than 10, cause that would just appear too close
    m_r = std::max(m_r - 0.2f, 10.0f);
}
void MoveLookController::ZoomOut(int mouseX, int mouseY)
{
    // For the game movelookController, zooming out just requires increasing m_r
    // Don't allow m_r to be greater than 65, cause that would just appear too far away
    m_r = std::min(m_r + 0.2f, 65.0f);
}

void MoveLookController::MouseMove()
{

}

void MoveLookController::UpdatePosition()
{
    // Treat arrow keys the same as WASD keys and don't double dip (meaning if 
    // up arrow is pressed and 'w' is pressed, don't go up twice...whatever that would mean)
    if (m_up || m_w)
    {
        // If DOWN arrow is also pressed, do nothing
        if (m_down || m_s)
            m_player->MoveForward(false);
        else if (m_shift)
        {
            LookUp();
            m_player->MoveForward(false);
        }
        else
            m_player->MoveForward(true);
    }
    else
    {
        m_player->MoveForward(false);

        if (m_down || m_s)
        {
            if (m_shift)
                LookDown();
            else
            {
                // Not actually sure what to do here. Do we want the player to spin around?
                int iii = 0;




            }
        }
    }

    if ((m_left || m_a) && !(m_right || m_d))
        LookLeft();
    else if ((m_right || m_d) && !(m_left || m_a))
        LookRight();
}

bool MoveLookController::IsMoving()
{
    return m_up || m_down || m_left || m_right; // || m_mouseDown; // || m_movingToNewLocation;
}

void MoveLookController::LookLeft()
{
    // Looking left is just spinning the camera in the negative theta direction
    float angle = static_cast<float>(m_turnSpeed * m_timeDelta);
    m_theta -= angle;

    // If shift is NOT down, then also rotate the player
    if (!m_shift)
        m_player->LookLeft(angle);
}
void MoveLookController::LookRight()
{
    // Looking right is just spinning the camera in the positive theta direction
    float angle = static_cast<float>(m_turnSpeed * m_timeDelta);
    m_theta += angle;

    // If shift is NOT down, then also rotate the player
    if (!m_shift)
        m_player->LookRight(angle);
}

void MoveLookController::LookLeftRight(float angle)
{
    // If the angle passed in is negative, it will look left, otherwise right
    m_theta += angle;
    UpdateCameraLocation();
}

void MoveLookController::LookUp()
{
    // LookUp is caused by SHIFT + UP arrow
    // This really just means move the camera closer to the y-axis, 
    // which is achieved by decreasing m_phi
    float angle = static_cast<float>(m_turnSpeed * m_timeDelta);
    m_phi = std::max(0.05f, m_phi - angle);         // don't let it get below 0.05
}
void MoveLookController::LookDown()
{
    // LookDown is caused by SHIFT + DOWN arrow
    // This really just means move the camera further from the positive y-axis, 
    // which is achieved by increasing m_phi
    float angle = static_cast<float>(m_turnSpeed * m_timeDelta);
    m_phi = std::min(DirectX::XM_PIDIV2, m_phi + angle);    // don't let it go below horizontal
}

void MoveLookController::LookUpDown(float angle)
{
    m_phi = std::min(DirectX::XM_PIDIV2, m_phi - angle);    // don't let it go below horizontal
    UpdateCameraLocation();
}