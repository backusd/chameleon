#include "Scene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd),
	m_cubePipeline(nullptr)
{
	// Create the move look controllers
	m_moveLookControllers.push_back(std::make_shared<MoveLookController>(m_hWnd));
	m_moveLookControllerIndex = 0;
	m_moveLookControllerIndexPrevious = 0;
#ifndef NDEBUG
	m_moveLookControllers.push_back(std::make_shared<FlyMoveLookController>(m_hWnd));
	m_moveLookControllers.push_back(std::make_shared<CenterOnOriginMoveLookController>(m_hWnd));
#endif

	CreateStaticResources();
	CreateWindowSizeDependentResources();

	m_frustum = std::make_shared<Frustum>(1000.0f, m_viewMatrix, m_projectionMatrix);

	// Cubes
	std::shared_ptr<Cube> cube = std::make_shared<Cube>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	cube->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	cube->SetSideLengths(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(cube);

	std::shared_ptr<Cube> cube2 = std::make_shared<Cube>(m_deviceResources, m_moveLookControllers[m_moveLookControllerIndex]);
	cube2->SetPosition(XMFLOAT3(5.0f, 0.0f, 0.0f));
	cube2->SetSideLengths(XMFLOAT3(1.0f, 2.0f, 1.0f));
	cube2->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(cube2);

	// 

	SetupTerrainPipeline();
	SetupTerrainCubePipeline();
	SetupSkyDomePipeline();
}

void Scene::CreateStaticResources()
{
	m_material = new PhongMaterialProperties();
	m_material->Material.Emissive = XMFLOAT4(0.4f, 0.14f, 0.14f, 1.0f);
	m_material->Material.Ambient = XMFLOAT4(1.0f, 0.75f, 0.75f, 1.0f);
	m_material->Material.Diffuse = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
	m_material->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_material->Material.SpecularPower = 6.0f;



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

void Scene::CreateWindowSizeDependentResources()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);

	m_viewport = CD3D11_VIEWPORT(
		static_cast<float>(rect.left), 
		static_cast<float>(rect.top),
		static_cast<float>(rect.right - rect.left),
		static_cast<float>(rect.bottom - rect.top)
	);


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

	// Set the view matrix
	m_viewMatrix = m_moveLookControllers[m_moveLookControllerIndex]->ViewMatrix();
}

void Scene::WindowResized()
{
	CreateWindowSizeDependentResources();

	// Update the bindables to know about the new projection matrix
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->SetProjectionMatrix(m_projectionMatrix);
}

void Scene::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
	// Update the move look control and get back the new view matrix
	m_moveLookControllers[m_moveLookControllerIndex]->Update(timer, keyboard, mouse);
	m_viewMatrix = m_moveLookControllers[m_moveLookControllerIndex]->ViewMatrix();

	// Update the frustum with the new view matrix
	m_frustum->UpdateFrustum(m_viewMatrix, m_projectionMatrix);

	//m_cubePipeline->Update(timer);
	// m_terrainPipeline->Update(timer);
	m_skyDomePipeline->Update(timer);

	// Update the sky dome's position to that of the move look controller
	XMFLOAT3 position;
	DirectX::XMStoreFloat3(&position, m_moveLookControllers[m_moveLookControllerIndex]->Position());
	m_skyDomePipeline->GetRenderable(0)->SetPosition(position);


	// Now that everything has been updated, perform terrain cell culling
	std::shared_ptr<TerrainMesh> terrain = ObjectStore::GetTerrain("terrain");
	std::shared_ptr<TerrainCellMesh> cell;
	for (int iii = 0; iii < terrain->TerrainCellCount(); ++iii)
	{
		cell = terrain->GetTerrainCell(iii);
		m_terrainCellVisibility[iii] = m_frustum->CheckRectangle2(
			cell->GetMaxWidth(), 
			cell->GetMaxHeight(), 
			cell->GetMaxDepth(), 
			cell->GetMinWidth(), 
			cell->GetMinHeight(), 
			cell->GetMinDepth()
		);
	}
	
	// Update all drawables
	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Update(timer);

	// If we are in DEBUG, then the move look controller may change, so update it 
#ifndef NDEBUG
	if (m_moveLookControllerIndexPrevious != m_moveLookControllerIndex)
	{
		m_moveLookControllerIndexPrevious = m_moveLookControllerIndex;

		for (std::shared_ptr<Drawable> drawable : m_drawables)
			drawable->SetMoveLookController(m_moveLookControllers[m_moveLookControllerIndex]);
	}
#endif


	//
	// Consider making this NDEBUG only
	//
	// m_moveLookController->UpdateImGui();


}


void Scene::Draw()
{
	// Draw the sky dome first because depth test will be turned off
	m_skyDomePipeline->Draw();


	for (std::shared_ptr<Drawable> drawable : m_drawables)
		drawable->Draw();


	// Draw each terrain cell that is visible
	for (unsigned int iii = 0; iii < m_terrainPipelines.size(); ++iii)
	{
		if (m_terrainCellVisibility[iii])
			m_terrainPipelines[iii]->Draw();
	}

	// m_terrainCubePipeline->Draw();
}

void Scene::SetupTerrainPipeline()
{
	std::shared_ptr<Terrain> terr = std::make_shared<Terrain>();

	std::shared_ptr<TerrainMesh> terrain = ObjectStore::GetTerrain("terrain");
	for (int iii = 0; iii < terrain->TerrainCellCount(); ++iii)
	{
		// Also populate the visibility vector
		m_terrainCellVisibility.push_back(false);

		std::ostringstream oss;
		oss << "terrain_" << iii;


		
		m_terrainPipelines.push_back(std::make_shared<DrawPipeline>(
			m_deviceResources,
			oss.str(),
			"terrain-texture-vertex-shader",
			"terrain-texture-pixel-shader",
			"solidfill",
			"depth-enabled-depth-stencil-state",
			"terrain-buffers-VS",
			"terrain-buffers-PS"
			));

		m_terrainPipelines[iii]->AddPixelShaderTextureArray("dirt-terrain-texture-array");

		m_terrainPipelines[iii]->SetSamplerState("terrain-texture-sampler");

		m_terrainPipelines[iii]->AddRenderable(terr);

		m_terrainPipelines[iii]->SetPerRendererableUpdate(
			[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
		(std::shared_ptr<Renderable> renderable,
			std::shared_ptr<Mesh> mesh,
			std::shared_ptr<ConstantBufferArray> vertexShaderBufferArray,
			std::shared_ptr<ConstantBufferArray> pixelShaderBufferArray)
		{
			auto deviceResources = weakDeviceResources.lock();
			ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

			D3D11_MAPPED_SUBRESOURCE ms;

			// Update the vertex shader buffer
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
			context->Map(vertexShaderBufferArray->GetRawBufferPointer(0), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

			XMMATRIX _model = renderable->GetModelMatrix();
			TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

			mappedBuffer->world = renderable->GetModelMatrix();
			mappedBuffer->view = this->ViewMatrix();
			mappedBuffer->projection = this->ProjectionMatrix();

			context->Unmap(vertexShaderBufferArray->GetRawBufferPointer(0), 0);


			// Update the pixel shader lighting buffer

			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
			context->Map(pixelShaderBufferArray->GetRawBufferPointer(0), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);


			// Get a pointer to the data in the light constant buffer.
			TerrainLightBufferType* lightingBuffer = (TerrainLightBufferType*)ms.pData;

			// Copy the lighting variables into the constant buffer.
			lightingBuffer->diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			lightingBuffer->lightDirection = XMFLOAT3(-0.5f, -1.0f, -0.5f);
			lightingBuffer->padding = 0.0f;

			// Unlock the light constant buffer.
			context->Unmap(pixelShaderBufferArray->GetRawBufferPointer(0), 0);
		}
		);
	}
}

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

void Scene::SetupSkyDomePipeline()
{
	m_skyDomePipeline = std::make_shared<DrawPipeline>(
		m_deviceResources,
		"sky-dome-mesh",
		"sky-dome-vertex-shader",
		"sky-dome-pixel-shader",
		"solidfill",			// Raster state will need to change
		"depth-disabled-depth-stencil-state",
		"sky-dome-buffers-VS",
		"sky-dome-buffers-PS"
		);


	m_skyDomePipeline->AddRenderable(std::make_shared<Terrain>());

	m_skyDomePipeline->SetPerRendererableUpdate(
		[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
	(std::shared_ptr<Renderable> renderable,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<ConstantBufferArray> vertexShaderBufferArray,
		std::shared_ptr<ConstantBufferArray> pixelShaderBufferArray)
	{
		auto deviceResources = weakDeviceResources.lock();
		ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

		std::shared_ptr<SkyDomeMesh> skyDomeMesh = std::dynamic_pointer_cast<SkyDomeMesh>(mesh);

		D3D11_MAPPED_SUBRESOURCE ms;

		// Update the vertex shader buffer
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(vertexShaderBufferArray->GetRawBufferPointer(0), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		XMMATRIX _model = renderable->GetModelMatrix();
		TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

		mappedBuffer->world = renderable->GetModelMatrix();
		mappedBuffer->view = this->ViewMatrix();
		mappedBuffer->projection = this->ProjectionMatrix();

		context->Unmap(vertexShaderBufferArray->GetRawBufferPointer(0), 0);


		// Update the pixel shader lighting buffer

		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(pixelShaderBufferArray->GetRawBufferPointer(0), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);


		// Get a pointer to the data in the light constant buffer.
		SkyDomeColorBufferType* skyDomeColorBuffer = (SkyDomeColorBufferType*)ms.pData;

		// Copy the lighting variables into the constant buffer.
		skyDomeColorBuffer->apexColor = skyDomeMesh->GetApexColor();
		skyDomeColorBuffer->centerColor = skyDomeMesh->GetCenterColor();

		// Unlock the light constant buffer.
		context->Unmap(pixelShaderBufferArray->GetRawBufferPointer(0), 0);
	}
	);
}


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
	}

	// Let the MoveLookController draw ImGui controls
	m_moveLookControllers[m_moveLookControllerIndex]->DrawImGui();
}