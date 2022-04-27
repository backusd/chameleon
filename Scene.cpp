#include "Scene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd),
	m_LButtonDown(false),
	m_RButtonDown(false),
	m_MButtonDown(false),
	m_mouseHoveredDrawable(nullptr),
	m_LPressDrawable(nullptr),
	m_RPressDrawable(nullptr),
	m_MPressDrawable(nullptr),
	m_distanceToHoveredDrawable(0.0f)
{
	// Create the move look controllers
	m_moveLookController = std::make_shared<MoveLookController>(m_hWnd, deviceResources);

#ifndef NDEBUG
	m_flyMoveLookController = std::make_shared<FlyMoveLookController>(m_hWnd, deviceResources);
	m_useFlyMoveLookController = false;
#endif

	CreateAndBindModelViewProjectionBuffer();

	// Terrain
	m_terrain = std::make_shared<Terrain>(m_deviceResources, m_moveLookController);
	
	m_terrain->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());

	// Must set the terrain to be used by the move look controller
	m_moveLookController->SetTerrain(m_terrain);
}

Scene::~Scene()
{
	// Manually release the player from the MoveLookController, otherwise, resources will be leaked
	// Same goes for the terrain
	m_moveLookController->ReleaseResources();
}

std::shared_ptr<Drawable> Scene::CreateDrawable()
{
	std::shared_ptr<Drawable> newItem = std::make_shared<Drawable>(m_deviceResources, m_moveLookController);
	newItem->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());
	m_drawables.push_back(newItem);
	return newItem;
}

std::shared_ptr<Player> Scene::CreatePlayer()
{
	std::shared_ptr<Player> player = std::make_shared<Player>(m_deviceResources, m_moveLookController);
	player->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());
	m_drawables.push_back(player);
	m_moveLookController->SetPlayer(player);
	return player;
}

void Scene::CreateAndBindModelViewProjectionBuffer()
{
	// Basically all objects that get rendered need to bind model/view/projection matrices
	// to the vertex shader. Instead of each object attempting to bind the same constant buffer,
	// the scene can set this buffer once at the start of the program and each update simply needs 
	// to update the members of the constant buffer that its vertex shader program will use

	INFOMAN(m_deviceResources);

	// Create a dynamic usage constant buffer that can be updated from the CPU
	std::shared_ptr<ConstantBuffer> modelViewProjectionConstantBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	modelViewProjectionConstantBuffer->CreateBuffer<ModelViewProjectionConstantBuffer>(
		D3D11_USAGE_DYNAMIC,			// Usage: Dynamic
		D3D11_CPU_ACCESS_WRITE,			// CPU Access: CPU will be able to write using Map
		0,								// Misc Flags: No miscellaneous flags
		0								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		// Not supplying any initial data
	);

	ID3D11Buffer* buffer[1] = { modelViewProjectionConstantBuffer->GetRawBufferPointer() };
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 1u, buffer)
	);
}

void Scene::WindowResized()
{
	// Must call this first because it will update the projection matrix
	m_moveLookController->WindowResized();

	// Update the bindables to know about the new projection matrix
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());

	m_terrain->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());
}

void Scene::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
	// Let the scene be responsible for processing mouse and keyboard input. This is simply easier
	// because some events don't need to be passed along to the move look controller and the scene
	// has access to the Drawables within the scene and can do things like mouse over testing
	ProcessMouseEvents(timer, mouse);
	ProcessKeyboardEvents(timer, keyboard);

	/*
	// Update the move look control and get back the new view matrix
#ifndef NDEBUG
	if (m_useFlyMoveLookController)
		m_flyMoveLookController->Update(timer, keyboard, mouse);
	else
		m_moveLookController->Update(timer, keyboard, mouse);
#else
	m_moveLookController->Update(timer, keyboard, mouse);
#endif
	*/

	// Update the terrain
	m_terrain->Update(timer);

	// Update all drawables
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Update(timer, m_terrain);


	// Update the location of the camera because it is possible the player has moved and therefore the
	// camera needs to follow
	m_moveLookController->UpdateCameraLocation();
}

void Scene::ProcessMouseEvents(std::shared_ptr<StepTimer> timer, std::shared_ptr<Mouse> mouse)
{
	std::shared_ptr<MoveLookController> mlc;
#ifndef NDEBUG	
	mlc = (m_useFlyMoveLookController) ? m_flyMoveLookController : m_moveLookController;
#else
	mlc = m_moveLookController;
#endif

	Mouse::Event e;
	while (!mouse->IsEmpty())
	{
		e = mouse->Read();
		switch (e.GetType())
		{
		case Mouse::Event::Type::WheelUp:	mlc->ZoomIn(mouse->GetPosX(), mouse->GetPosY()); break;
		case Mouse::Event::Type::WheelDown: mlc->ZoomOut(mouse->GetPosX(), mouse->GetPosY()); break;

		case Mouse::Event::Type::LPress:	
			// On LPress, keep track of what object the press down was on as well as the initial click coordinates
			
			break;
		case Mouse::Event::Type::LRelease:	
			// On LRelease, if the user was just pulling the screen to turn the character, do nothing
			// If the Drawable selected from the LPress event is still the same as the hovered object, 
			//		call Drawable->OnClick event for that object
			
			break;

		case Mouse::Event::Type::RPress:	break; // Do the same as LRelease, but keep separate variables
		case Mouse::Event::Type::RRelease:  break;

		case Mouse::Event::Type::MPress:	break; // Do the same as LRelease, but keep separate variables
		case Mouse::Event::Type::MRelease:	break;

		case Mouse::Event::Type::Move:		
			// On a Move event, we need to determine what the mouse is over
			// If L/M/R Buttons are all NOT down, then call Drawable->OnMouseHover for the selected object (if not null)
			if (!(m_LButtonDown || m_RButtonDown || m_MButtonDown))
			{
				m_mouseHoveredDrawable = nullptr;
				float shortestDistance = FLT_MAX;

				for (std::shared_ptr<Drawable> drawable : m_drawables)
				{
					if (drawable->IsMouseHovered(mouse->GetPosX(), mouse->GetPosY(), m_distanceToHoveredDrawable))
					{
						if (m_distanceToHoveredDrawable < shortestDistance)
						{
							shortestDistance = m_distanceToHoveredDrawable;
							m_mouseHoveredDrawable = drawable;
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void Scene::ProcessKeyboardEvents(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard)
{
	// The MoveLookController should process events specifically for the purpose of moving the
	// camera in the scene. Therefore, there are several events, such as just moving the mouse
	// without having clicked, that the scene itself should be responsible for handling. So, allow
	// the MoveLookController to peek at the mouse events that are present first. If there are events
	// that the MoveLookController does not want to handle, process them here (MAKE SURE TO EMPTY
	// THE EVENT QUEUE OTHERWISE THE EVENTS WILL PERSIST)
	/*
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
	*/
}

void Scene::Draw()
{
	// Draw all drawables - NOTE: If using a SkyDome, it MUST be the first drawable
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Draw();

	m_terrain->Draw();
}

void Scene::Activate()
{
	// When activating a new scene, you must activate the scene lighting
	m_lighting->Activate();
}

#ifndef NDEBUG
void Scene::DrawImGui()
{
	// Draw a view mode selector control
	ImGui::Begin("View Mode");

	if (ImGui::RadioButton("Player Mode", !m_useFlyMoveLookController))
	{
		m_useFlyMoveLookController = false;
		UpdateMoveLookControllerSelection();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Fly Mode", m_useFlyMoveLookController))
	{
		m_useFlyMoveLookController = true;
		UpdateMoveLookControllerSelection();
	}

	ImGui::End();


	// Let the MoveLookController draw ImGui controls
	if (m_useFlyMoveLookController)
		m_flyMoveLookController->DrawImGui();

	// Display a menu of all objects in the scene
	ImGui::Begin("Object Edit");

	// Easiest way to make each drawable unique is to pass the number of the drawable to the DrawImGui function
	for (unsigned int iii = 0; iii < m_drawables.size(); ++iii)
		m_drawables[iii]->DrawImGui(std::to_string(iii));

	ImGui::End();
}

void Scene::UpdateMoveLookControllerSelection()
{
	std::shared_ptr<MoveLookController> mlc = (m_useFlyMoveLookController) ? m_flyMoveLookController : m_moveLookController;

	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->SetMoveLookController(mlc);

	m_terrain->SetMoveLookController(mlc);
}
#endif