#include "Terrain.h"

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;
using DirectX::XMMATRIX;

Terrain::Terrain(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController)
{
	// Create the frustum (will have bad values to start but will get updated later
	m_frustum = std::make_shared<Frustum>(1000.0f, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity());

	m_terrainMesh = ObjectStore::GetTerrainMesh("terrain-mesh");
	for (int iii = 0; iii < m_terrainMesh->TerrainCellCount(); ++iii)
	{
		// Also populate the visibility vector
		m_terrainCellVisibility.push_back(false);

		std::ostringstream oss;
		oss << "terrain_" << iii;

		m_terrainCells.push_back(std::make_shared<TerrainCell>(deviceResources, moveLookController, oss.str()));
	}


	// Can bind everything once that will be the same for each cell
	AddBindable("terrain-texture-vertex-shader");		// Vertex Shader
	AddBindable("terrain-texture-vertex-shader-IA");	// Input Layout
	AddBindable("terrain-texture-pixel-shader");		// Pixel Shader
	AddBindable("solidfill"); //"wireframe",			// Rasterizer State
	AddBindable("depth-enabled-depth-stencil-state");	// Depth Stencil State
	//AddBindable("terrain-buffers-VS");					// VS Constant buffers - Are controlled by Terrain
	// AddBindable("terrain-buffers-PS");					// PS Constant buffers - Are controlled by Terrain
	AddBindable("dirt-terrain-texture-array");			// Terrain Textures for PS
	AddBindable("terrain-texture-sampler");				// Sampler State





	TerrainLightBufferType lightData;
	lightData.diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	lightData.lightDirection = XMFLOAT3(-0.5f, -1.0f, -0.5f);
	lightData.padding = 0.0f;

	// Create an immutable constant buffer and load it with the lighting
	std::shared_ptr<ConstantBuffer> lightingBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	lightingBuffer->CreateBuffer<TerrainLightBufferType>(
		D3D11_USAGE_IMMUTABLE,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&lightData)	// Initial Data: Fill the buffer with lighting data
		);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);

	// Add the lighting constant buffer
	psConstantBufferArray->AddBuffer(lightingBuffer);

	m_bindables.push_back(psConstantBufferArray);
}

void Terrain::SetProjectionMatrix(DirectX::XMMATRIX matrix)
{
	m_frustum->UpdateFrustum(m_moveLookController->ViewMatrix(), matrix);

	for (std::shared_ptr<TerrainCell> cell : m_terrainCells)
		cell->SetProjectionMatrix(matrix);
}

void Terrain::Update(std::shared_ptr<StepTimer> timer)
{
	m_frustum->UpdateFrustum(m_moveLookController->ViewMatrix(), m_terrainCells[0]->GetProjectionMatrix());

	std::shared_ptr<TerrainCellMesh> cell;
	for (int iii = 0; iii < m_terrainMesh->TerrainCellCount(); ++iii)
	{
		cell = m_terrainMesh->GetTerrainCell(iii);
		m_terrainCellVisibility[iii] = m_frustum->CheckRectangle2(
			cell->GetMaxWidth(),
			cell->GetMaxHeight(),
			cell->GetMaxDepth(),
			cell->GetMinWidth(),
			cell->GetMinHeight(),
			cell->GetMinDepth()
		);
	}
}

void Terrain::Draw()
{
	for (std::shared_ptr<Bindable> bindable : m_bindables)
		bindable->Bind();

	UpdateBindings();

	for (unsigned int iii = 0; iii < m_terrainCells.size(); ++iii)
	{
		if (m_terrainCellVisibility[iii])
			m_terrainCells[iii]->Draw();
	}
}

void Terrain::UpdateBindings()
{
	// Update the vertex shader constant buffer

	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Update VS constant buffer with model/view/projection info
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsBuffer;
	GFX_THROW_INFO_ONLY(
		context->VSGetConstantBuffers(0, 1, vsBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(vsBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;
	XMMATRIX model = DirectX::XMMatrixIdentity();
	XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * m_terrainCells[0]->GetProjectionMatrix();
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), model);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), model * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);
}

#ifndef NDEBUG
void Terrain::SetMoveLookController(std::shared_ptr<MoveLookController> mlc) 
{ 
	m_moveLookController = mlc;

	for (std::shared_ptr<TerrainCell> cell : m_terrainCells)
		cell->SetMoveLookController(mlc);
}
#endif