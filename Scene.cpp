#include "Scene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd)
{
	// Create the move look controllers
	m_moveLookControllers.push_back(std::make_shared<MoveLookController>(m_hWnd));
	m_moveLookControllerIndex = 0;
	m_moveLookControllerIndexPrevious = 0;
#ifndef NDEBUG
	m_moveLookControllers.push_back(std::make_shared<FlyMoveLookController>(m_hWnd));
	m_moveLookControllers.push_back(std::make_shared<CenterOnOriginMoveLookController>(m_hWnd));
#endif

	//CreateStaticResources();
	CreateWindowSizeDependentResources();
	//CreateAndBindLightPropertiesBuffer();
	CreateAndBindModelViewProjectionBuffer();

	// Sky Dome
	//     MUST be added first because it needs to be rendered first because depth test is turned off
	std::shared_ptr<SkyDome> skyDome = std::make_shared<SkyDome>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	skyDome->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(skyDome);

	// Lighting
	//		Lighting should be draw second because it will update PS constant buffers that will be required for other objects
	m_lighting = std::make_shared<Lighting>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	m_lighting->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(m_lighting);

	// Sphere
	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	sphere->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(sphere);

	// Cubes
	std::shared_ptr<Box> box1 = std::make_shared<Box>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	box1->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	box1->SetSideLengths(XMFLOAT3(1.0f, 1.0f, 1.0f));
	box1->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(box1);

	std::shared_ptr<Box> box2 = std::make_shared<Box>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	box2->SetPosition(XMFLOAT3(5.0f, 0.0f, 0.0f));
	box2->SetSideLengths(XMFLOAT3(1.0f, 2.0f, 1.0f));
	box2->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(box2);

	// Suzanne
	std::shared_ptr<Suzanne> suzanne = std::make_shared<Suzanne>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	suzanne->SetProjectionMatrix(m_projectionMatrix);
	//m_drawables.push_back(suzanne);

	// Terrain
	m_terrain = std::make_shared<Terrain>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	m_terrain->SetProjectionMatrix(m_projectionMatrix);

}
/*
void Scene::CreateStaticResources()
{
	m_lightProperties = LightProperties();
	m_lightProperties.GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	// The initial eye position - you will want to modify MoveLookController so the Eye
	// position can be retrieved to also update the light position
	m_lightProperties.EyePosition = XMFLOAT4(0.0f, 0.0f, -2.0f, 0.0f);

	// Add the lights
	static const XMVECTORF32 LightColors[MAX_LIGHTS] = {
		DirectX::Colors::White,
		DirectX::Colors::Orange,
		DirectX::Colors::Yellow,
		DirectX::Colors::Green,
		DirectX::Colors::Blue,
		DirectX::Colors::Indigo,
		DirectX::Colors::Violet,
		DirectX::Colors::White
	};

	static const LightType LightTypes[MAX_LIGHTS] = {
		PointLight, SpotLight, SpotLight, PointLight, SpotLight, SpotLight, SpotLight, PointLight
	};

	static const bool LightEnabled[MAX_LIGHTS] = {
		true, false, false, false, false, false, false, false
	};

	const int numLights = MAX_LIGHTS;
	for (int i = 0; i < numLights; ++i)
	{
		Light light;
		light.Enabled = static_cast<int>(LightEnabled[i]);
		light.LightType = LightTypes[i];
		light.Color = XMFLOAT4(LightColors[i]);
		light.SpotAngle = DirectX::XMConvertToRadians(45.0f);
		light.ConstantAttenuation = 1.0f;
		light.LinearAttenuation = 0.08f;
		light.QuadraticAttenuation = 0.0f;

		// Make the light slightly offset from the initial eye position
		//XMFLOAT4 LightPosition = XMFLOAT4(std::sin(totalTime + offset * i) * radius, 9.0f, std::cos(totalTime + offset * i) * radius, 1.0f);
		XMFLOAT4 LightPosition = XMFLOAT4(0.0f, 1.0f, -2.0f, 1.0f);
		light.Position = LightPosition;
		XMVECTOR LightDirection = DirectX::XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
		XMStoreFloat4(&light.Direction, DirectX::XMVector3Normalize(LightDirection));

		m_lightProperties.Lights[i] = light;
	}
}
*/

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

/*
void Scene::CreateAndBindLightPropertiesBuffer()
{
	// The scene will be responsible for keeping track of all lights in the scene
	// To reduce the number of times that the lighting data gets bound to the pipeline,
	// the scene can simply bind the lighting data constant buffer once at program launch
	// and then only update the buffer when necessary. ALL PS shader programs will need to 
	// be aware that the lighting constant buffer is bound to slot 0 and they should bind
	// additional buffers starting at slot 1

	INFOMAN(m_deviceResources);

	// Create a default usage constant buffer and load it with the material data
	// NOTE: Because it is not CPU writeable, it MUST be updated using UpdateSubresource (cannot use Map)
	std::shared_ptr<ConstantBuffer> lightConstantBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	lightConstantBuffer->CreateBuffer<LightProperties>(
		D3D11_USAGE_DEFAULT,			// Usage: Default - will not be CPU writeable
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&m_lightProperties)	// Initial Data: Fill the buffer with light data
		);

	
	ID3D11Buffer* buffer[1] = { lightConstantBuffer->GetRawBufferPointer() };
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 1u, buffer)
	);
}
*/

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
	// If we are in DEBUG, then the move look controller may change, so update it 
#ifndef NDEBUG
	if (m_moveLookControllerIndexPrevious != m_moveLookControllerIndex)
	{
		m_moveLookControllerIndexPrevious = m_moveLookControllerIndex;

		for (std::shared_ptr<Drawable> drawable : m_drawables)
			drawable->SetMoveLookController(m_moveLookControllers[m_moveLookControllerIndex]);

		m_terrain->SetMoveLookController(m_moveLookControllers[m_moveLookControllerIndex]);
	}
#endif

	// Update the move look control and get back the new view matrix
	m_moveLookControllers[m_moveLookControllerIndex]->Update(timer, keyboard, mouse);
	
	// Update all drawables
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Update(timer);

	// Update the terrain
	m_terrain->Update(timer);
}

void Scene::Draw()
{
	// Draw all drawables - NOTE: If using a SkyDome, it MUST be the first drawable
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Draw();

	m_terrain->Draw();
}

/*
void Scene::SetupTerrainCubePipeline()
{
	
	m_terrainCubePipeline = std::make_shared<DrawPipeline>(
		m_deviceResources,
		"box-outline-mesh",
		"solid-vertex-shader",
		"solid-pixel-shader",
		"solidfill",
		"depth-enabled-depth-stencil-state",
		"terrain-cube-buffers-VS"
		);

	std::shared_ptr<TerrainMesh> terrain = ObjectStore::GetTerrain("terrain");
	std::shared_ptr<TerrainCellMesh> cell;
	for (int iii = 0; iii < terrain->TerrainCellCount(); ++iii)
	{
		cell = terrain->GetTerrainCell(iii);
		m_terrainCubePipeline->AddRenderable(
			std::make_shared<Box>(cell->GetCenter(), cell->GetXLength(), cell->GetYLength(), cell->GetZLength())
		);
	}

	m_terrainCubePipeline->SetPerRendererableUpdate(
		[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
	(std::shared_ptr<Renderable> renderable,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<ConstantBufferArray> vertexShaderBufferArray,
		std::shared_ptr<ConstantBufferArray> pixelShaderBufferArray)
	{
		auto deviceResources = weakDeviceResources.lock();
		ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE ms;

		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(vertexShaderBufferArray->GetRawBufferPointer(0), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		XMMATRIX _model = renderable->GetModelMatrix();
		TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

		mappedBuffer->world = renderable->GetModelMatrix();
		mappedBuffer->view = this->ViewMatrix();
		mappedBuffer->projection = this->ProjectionMatrix();

		context->Unmap(vertexShaderBufferArray->GetRawBufferPointer(0), 0);
	}
	);
}
*/

void Scene::DrawImGui()
{
	// Draw a view mode selector control
	ImGui::Begin("View Mode");
	ImGui::RadioButton("Player Mode", &m_moveLookControllerIndex, 0);
	ImGui::RadioButton("Fly Mode", &m_moveLookControllerIndex, 1);
	ImGui::RadioButton("Center On Origin Mode", &m_moveLookControllerIndex, 2);
	ImGui::End();

	// If viewing mode is center on origin, then draw the object edit control panel
	if (m_moveLookControllerIndex == 2)
	{
		ImGui::Begin("Object Edit");

		ImGui::End();

		// Draw the Center-on-origin lighting control menu
		m_lighting->DrawImGui();
	}

	// Let the MoveLookController draw ImGui controls
	m_moveLookControllers[m_moveLookControllerIndex]->DrawImGui();	
}