#include "Scene.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd) :
	m_deviceResources(deviceResources),
	m_hWnd(hWnd),
	m_cubePipeline(nullptr)
{
	m_moveLookController = std::make_unique<MoveLookController>();
	m_moveLookController->SetPosition(XMFLOAT3(50.0f, 10.0f, -10.0f));

	CreateStaticResources();
	CreateWindowSizeDependentResources();


	SetupCubePipeline();
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
	m_viewMatrix = m_moveLookController->ViewMatrix();
}

void Scene::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse)
{
	// Update the move look control and get back the new view matrix
	m_moveLookController->Update(timer, keyboard, mouse, m_hWnd);
	m_viewMatrix = m_moveLookController->ViewMatrix();


	m_cubePipeline->Update(timer);
	// m_terrainPipeline->Update(timer);
	m_skyDomePipeline->Update(timer);

	// Update the sky dome's position to that of the move look controller
	XMFLOAT3 position;
	DirectX::XMStoreFloat3(&position, m_moveLookController->Position());
	m_skyDomePipeline->GetRenderable(0)->SetPosition(position);
}


void Scene::Draw()
{
	// Draw the sky dome first because depth test will be turned off
	m_skyDomePipeline->Draw();

	// Set the cube material once for all cubes then draw them
	m_cubePipeline->UpdatePSSubresource(0, m_material);
	m_cubePipeline->Draw();

	// Draw the terrain
	for (std::shared_ptr<DrawPipeline> terrainPipeline : m_terrainPipelines)
		terrainPipeline->Draw();

	m_terrainCubePipeline->Draw();
}

void Scene::SetupCubePipeline()
{
	std::vector<std::string> vertexConstantBuffers = { "model-view-projection-buffer" };
	std::vector<std::string> pixelConstantBuffers = { "phong-material-properties-buffer", "light-properties-buffer" };

	m_cubePipeline = std::make_shared<DrawPipeline>(
		m_deviceResources,
		"box-filled-mesh",
		"phong-vertex-shader",
		"phong-pixel-shader",
		"solidfill",
		"depth-enabled-depth-stencil-state",
		vertexConstantBuffers,
		pixelConstantBuffers
		);

	m_cubePipeline->AddRenderable(std::make_shared<Box>(1.0f));
	m_cubePipeline->AddRenderable(std::make_shared<Box>(XMFLOAT3(2.0f, 0.0f, 0.0f), 0.5f));

	// Try to set the light properties only once at the beginning of the application
	m_cubePipeline->UpdatePSSubresource(1, &m_lightProperties);

	m_cubePipeline->SetPerRendererableUpdate(
		[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
	(std::shared_ptr<Renderable> renderable,
		std::shared_ptr<Mesh> mesh,
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& vertexShaderBuffers,
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pixelShaderBuffers)
	{
		auto deviceResources = weakDeviceResources.lock();
		ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE ms;

		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(vertexShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		XMMATRIX _model = renderable->GetModelMatrix();
		ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;

		DirectX::XMStoreFloat4x4(&(mappedBuffer->model), _model);
		DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), _model* this->ViewProjectionMatrix());
		DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, _model)));

		context->Unmap(vertexShaderBuffers[0].Get(), 0);
	}
	);
}

void Scene::SetupTerrainPipeline()
{
	std::vector<std::string> vertexConstantBuffers = { "terrain-constant-buffer" };
	std::vector<std::string> pixelConstantBuffers = { "terrain-light-buffer" };

	std::shared_ptr<Terrain> terr = std::make_shared<Terrain>();

	std::shared_ptr<TerrainMesh> terrain = ObjectStore::GetTerrain("terrain");
	for (int iii = 0; iii < terrain->TerrainCellCount(); ++iii)
	{
		std::ostringstream oss;
		oss << "terrain_" << iii;


		
		m_terrainPipelines.push_back(std::make_shared<DrawPipeline>(
			m_deviceResources,
			oss.str(),
			"terrain-texture-vertex-shader",
			"terrain-texture-pixel-shader",
			"solidfill",
			"depth-enabled-depth-stencil-state",
			vertexConstantBuffers,
			pixelConstantBuffers
			));

		m_terrainPipelines[iii]->AddPixelShaderTexture("terrain-texture");
		m_terrainPipelines[iii]->AddPixelShaderTexture("terrain-normal-map-texture");

		m_terrainPipelines[iii]->SetSamplerState("terrain-texture-sampler");

		m_terrainPipelines[iii]->AddRenderable(terr);

		m_terrainPipelines[iii]->SetPerRendererableUpdate(
			[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
		(std::shared_ptr<Renderable> renderable,
			std::shared_ptr<Mesh> mesh,
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& vertexShaderBuffers,
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pixelShaderBuffers)
		{
			auto deviceResources = weakDeviceResources.lock();
			ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

			D3D11_MAPPED_SUBRESOURCE ms;

			// Update the vertex shader buffer
			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
			context->Map(vertexShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

			XMMATRIX _model = renderable->GetModelMatrix();
			TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

			mappedBuffer->world = renderable->GetModelMatrix();
			mappedBuffer->view = this->ViewMatrix();
			mappedBuffer->projection = this->ProjectionMatrix();

			context->Unmap(vertexShaderBuffers[0].Get(), 0);


			// Update the pixel shader lighting buffer

			ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
			context->Map(pixelShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);


			// Get a pointer to the data in the light constant buffer.
			TerrainLightBufferType* lightingBuffer = (TerrainLightBufferType*)ms.pData;

			// Copy the lighting variables into the constant buffer.
			lightingBuffer->diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			lightingBuffer->lightDirection = XMFLOAT3(-0.5f, -1.0f, -0.5f);
			lightingBuffer->padding = 0.0f;

			// Unlock the light constant buffer.
			context->Unmap(pixelShaderBuffers[0].Get(), 0);
		}
		);
	}
}

void Scene::SetupTerrainCubePipeline()
{
	std::vector<std::string> vertexConstantBuffers = { "terrain-constant-buffer" };
	std::vector<std::string> pixelConstantBuffers = { };

	m_terrainCubePipeline = std::make_shared<DrawPipeline>(
		m_deviceResources,
		"box-outline-mesh",
		"solid-vertex-shader",
		"solid-pixel-shader",
		"solidfill",
		"depth-enabled-depth-stencil-state",
		vertexConstantBuffers,
		pixelConstantBuffers
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
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& vertexShaderBuffers,
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pixelShaderBuffers)
	{
		auto deviceResources = weakDeviceResources.lock();
		ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

		D3D11_MAPPED_SUBRESOURCE ms;

		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(vertexShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		XMMATRIX _model = renderable->GetModelMatrix();
		TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

		mappedBuffer->world = renderable->GetModelMatrix();
		mappedBuffer->view = this->ViewMatrix();
		mappedBuffer->projection = this->ProjectionMatrix();

		context->Unmap(vertexShaderBuffers[0].Get(), 0);
	}
	);
}


void Scene::SetupSkyDomePipeline()
{
	std::vector<std::string> vertexConstantBuffers = { "sky-dome-constant-buffer" };
	std::vector<std::string> pixelConstantBuffers = { "sky-dome-gradient-buffer" };

	m_skyDomePipeline = std::make_shared<DrawPipeline>(
		m_deviceResources,
		"sky-dome-mesh",
		"sky-dome-vertex-shader",
		"sky-dome-pixel-shader",
		"solidfill",			// Raster state will need to change
		"depth-disabled-depth-stencil-state",
		vertexConstantBuffers,
		pixelConstantBuffers
		);


	m_skyDomePipeline->AddRenderable(std::make_shared<Terrain>());

	m_skyDomePipeline->SetPerRendererableUpdate(
		[this, weakDeviceResources = std::weak_ptr<DeviceResources>(m_deviceResources)]
	(std::shared_ptr<Renderable> renderable,
		std::shared_ptr<Mesh> mesh,
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& vertexShaderBuffers,
		std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>& pixelShaderBuffers)
	{
		auto deviceResources = weakDeviceResources.lock();
		ID3D11DeviceContext4* context = deviceResources->D3DDeviceContext();

		std::shared_ptr<SkyDomeMesh> skyDomeMesh = std::dynamic_pointer_cast<SkyDomeMesh>(mesh);

		D3D11_MAPPED_SUBRESOURCE ms;

		// Update the vertex shader buffer
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(vertexShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		XMMATRIX _model = renderable->GetModelMatrix();
		TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

		mappedBuffer->world = renderable->GetModelMatrix();
		mappedBuffer->view = this->ViewMatrix();
		mappedBuffer->projection = this->ProjectionMatrix();

		context->Unmap(vertexShaderBuffers[0].Get(), 0);


		// Update the pixel shader lighting buffer

		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(pixelShaderBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);


		// Get a pointer to the data in the light constant buffer.
		SkyDomeColorBufferType* skyDomeColorBuffer = (SkyDomeColorBufferType*)ms.pData;

		// Copy the lighting variables into the constant buffer.
		skyDomeColorBuffer->apexColor = skyDomeMesh->GetApexColor();
		skyDomeColorBuffer->centerColor = skyDomeMesh->GetCenterColor();

		// Unlock the light constant buffer.
		context->Unmap(pixelShaderBuffers[0].Get(), 0);
	}
	);
}