#include "MoveLookController.h"
#include "Nanosuit.h"

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;

using DirectX::operator*;

MoveLookController::MoveLookController(HWND hWnd) :
    m_hWnd(hWnd),
    m_moveSpeed(10.0),
    m_turnSpeed(0.5),
    m_player(nullptr),
    m_r(35.0f),
    m_theta(0.0f),
    m_phi(DirectX::XM_PI / 2.5f) // 2.5 is somewhat random and just gives a pleasing angle for the camera
{
    ResetState();

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
    
    //m_movingToNewLocation = false;
    //m_updatedViewMatrixHasBeenRead = false;
    //m_rotatingLeftRight = false;
    //m_rotatingUpDown = false;
}

DirectX::XMMATRIX MoveLookController::ViewMatrix()
{
    return DirectX::XMMatrixLookAtRH(m_eyeVec, m_atVec, m_upVec);
}

void MoveLookController::SetPlayer(std::shared_ptr<Nanosuit> player) 
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
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);
    m_theta -= angle;

    // If shift is NOT down, then also rotate the player
    if (!m_shift)
        m_player->LookLeft(angle);
}
void MoveLookController::LookRight()
{
    // Looking right is just spinning the camera in the positive theta direction
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);
    m_theta += angle;

    // If shift is NOT down, then also rotate the player
    if (!m_shift)
        m_player->LookRight(angle);
}
void MoveLookController::LookUp()
{
    // LookUp is caused by SHIFT + UP arrow
    // This really just means move the camera closer to the y-axis, 
    // which is achieved by decreasing m_phi
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);
    m_phi = std::max(0.05f, m_phi - angle);         // don't let it get below 0.05
}
void MoveLookController::LookDown()
{
    // LookDown is caused by SHIFT + DOWN arrow
    // This really just means move the camera further from the positive y-axis, 
    // which is achieved by increasing m_phi
    double timeDelta = m_currentTime - m_previousTime;
    float angle = static_cast<float>(m_turnSpeed * timeDelta);
    m_phi = std::min(DirectX::XM_PIDIV2, m_phi + angle);    // don't let it go below horizontal
}
