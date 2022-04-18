#include "Scene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd)
{
	// Create the move look controllers
	m_moveLookController = std::make_shared<MoveLookController>(m_hWnd);

#ifndef NDEBUG
	m_flyMoveLookController = std::make_shared<FlyMoveLookController>(m_hWnd);
	m_useFlyMoveLookController = false;
#endif

	CreateWindowSizeDependentResources();
	CreateAndBindModelViewProjectionBuffer();

	// Terrain
	m_terrain = std::make_shared<Terrain>(m_deviceResources, m_moveLookController);
	m_terrain->SetProjectionMatrix(m_projectionMatrix);
}

Scene::~Scene()
{
	// Manually release the player from the MoveLookController, otherwise, resources will be leaked
	m_moveLookController->ReleasePlayer();
}

void Scene::CreateWindowSizeDependentResources()
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
	CreateWindowSizeDependentResources();

	// Update the bindables to know about the new projection matrix
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->SetProjectionMatrix(m_projectionMatrix);

	m_terrain->SetProjectionMatrix(m_projectionMatrix);
}

void Scene::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
	// Update the move look control and get back the new view matrix
#ifndef NDEBUG
	if (m_useFlyMoveLookController)
		m_flyMoveLookController->Update(timer, keyboard, mouse);
	else
		m_moveLookController->Update(timer, keyboard, mouse);
#else
	m_moveLookController->Update(timer, keyboard, mouse);
#endif

	// Update the terrain
	m_terrain->Update(timer);

	// Update all drawables
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Update(timer, m_terrain);
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