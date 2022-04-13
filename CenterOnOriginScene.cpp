#include "CenterOnOriginScene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

CenterOnOriginScene::CenterOnOriginScene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd)
{
	m_moveLookController = std::make_shared<CenterOnOriginMoveLookController>(m_hWnd);

	CreateWindowSizeDependentResources();
	CreateAndBindModelViewProjectionBuffer();

	// Sky Dome
	//     MUST be added first because it needs to be rendered first because depth test is turned off
	std::shared_ptr<SkyDome> skyDome = std::make_shared<SkyDome>(m_deviceResources, m_moveLookController);
	skyDome->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(skyDome);

	// Lighting
	//		Lighting should be draw second because it will update PS constant buffers that will be required for other objects
	m_lighting = std::make_shared<Lighting>(m_deviceResources, m_moveLookController);
	m_lighting->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(m_lighting);

	// Sphere
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(m_deviceResources, m_moveLookController);
	sphere->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(sphere);

	// Cubes
	std::shared_ptr<Box> box1 = std::make_shared<Box>(m_deviceResources, m_moveLookController);
	box1->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	box1->SetSideLengths(XMFLOAT3(1.0f, 1.0f, 1.0f));
	box1->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(box1);

	std::shared_ptr<Box> box2 = std::make_shared<Box>(m_deviceResources, m_moveLookController);
	box2->SetPosition(XMFLOAT3(5.0f, 0.0f, 0.0f));
	box2->SetSideLengths(XMFLOAT3(1.0f, 2.0f, 1.0f));
	box2->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(box2);

	// Suzanne
	std::shared_ptr<Suzanne> suzanne = std::make_shared<Suzanne>(m_deviceResources, m_moveLookController);
	suzanne->SetProjectionMatrix(m_projectionMatrix);
	suzanne->SetPosition(XMFLOAT3(0.0f, 0.0f, 3.0f));
	//m_drawables.push_back(suzanne);

	std::shared_ptr<Suzanne> suzanne2 = std::make_shared<Suzanne>(m_deviceResources, m_moveLookController);
	suzanne2->SetProjectionMatrix(m_projectionMatrix);
	suzanne2->SetPosition(XMFLOAT3(3.0f, 0.0f, 3.0f));
	//m_drawables.push_back(suzanne2);

	// Nanosuit
	std::shared_ptr<Nanosuit> nanosuit = std::make_shared<Nanosuit>(m_deviceResources, m_moveLookController);
	nanosuit->SetProjectionMatrix(m_projectionMatrix);
	nanosuit->SetPosition(XMFLOAT3(0.0f, -5.0f, 0.0f));
	m_drawables.push_back(nanosuit);

	std::shared_ptr<Nanosuit> nanosuit2 = std::make_shared<Nanosuit>(m_deviceResources, m_moveLookController);
	nanosuit2->SetProjectionMatrix(m_projectionMatrix);
	nanosuit2->SetPosition(XMFLOAT3(10.0f, -5.0f, 0.0f));
	m_drawables.push_back(nanosuit2);

}

void CenterOnOriginScene::CreateWindowSizeDependentResources()
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

void CenterOnOriginScene::CreateAndBindModelViewProjectionBuffer()
{
	// Basically all objects that get rendered need to bind model/view/projection matrices
	// to the vertex shader. Instead of each object attempting to bind the same constant buffer,
	// the scene can set this buffer once at the start of the program and each update simply needs 
	// to update the members of the constant buffer that its vertex shader program will use

	//
	// I don't think we need to do this because it will be done by the Scene
	//
	/*
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
	*/
}

void CenterOnOriginScene::WindowResized()
{
	CreateWindowSizeDependentResources();

	// Update the bindables to know about the new projection matrix
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->SetProjectionMatrix(m_projectionMatrix);
}

void CenterOnOriginScene::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
	// Update the move look control and get back the new view matrix
	m_moveLookController->Update(timer, keyboard, mouse);

	// Update all drawables
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Update(timer);
}

void CenterOnOriginScene::Draw()
{
	// Draw all drawables - NOTE: If using a SkyDome, it MUST be the first drawable
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Draw();
}

void CenterOnOriginScene::DrawImGui()
{
	// Draw a view mode selector control
	ImGui::Begin("Center On Origin");
	ImGui::Text("Could add controls to add/remove/edit objects to the");
	ImGui::Text("Center On Origin scene.");
	ImGui::End();


	// Display a menu of all objects in the scene
	ImGui::Begin("Object Edit");

	// Easiest way to make each drawable unique is to pass the number of the drawable to the DrawImGui function
	for (unsigned int iii = 0; iii < m_drawables.size(); ++iii)
		m_drawables[iii]->DrawImGui(std::to_string(iii));

	ImGui::End();


	// Let the MoveLookController draw ImGui controls
	m_moveLookController->DrawImGui();
}