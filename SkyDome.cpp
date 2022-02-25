#include "SkyDome.h"

SkyDome::SkyDome(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController),
	m_apexColor(XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f)),
	m_centerColor(XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f))
{
	SetMesh("sky-dome-mesh");

	AddBindable("sky-dome-vertex-shader");				// Vertex Shader
	AddBindable("sky-dome-vertex-shader-IA");			// Input Layout
	AddBindable("sky-dome-pixel-shader");				// Pixel Shader
	AddBindable("solidfill"); //"wireframe",			// Rasterizer State
	AddBindable("depth-disabled-depth-stencil-state");	// Depth Stencil State
	AddBindable("sky-dome-buffers-VS");					// VS Constant buffers
	//AddBindable("sky-dome-buffers-PS");					// PS Constant buffers

	CreateAndAddPSBufferArray();
}

void SkyDome::CreateAndAddPSBufferArray()
{
	SkyDomeColorBufferType skyDomeColorBuffer;
	skyDomeColorBuffer.apexColor = m_apexColor;
	skyDomeColorBuffer.centerColor = m_centerColor;

	// Create an immutable constant buffer and load it with the sky color
	std::shared_ptr<ConstantBuffer> skyColorBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	skyColorBuffer->CreateBuffer<SkyDomeColorBufferType>(
		D3D11_USAGE_IMMUTABLE,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&skyDomeColorBuffer)	// Initial Data: Fill the buffer with sky dome color data
		);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);

	// Add the color constant buffer
	psConstantBufferArray->AddBuffer(skyColorBuffer);

	m_bindables.push_back(psConstantBufferArray);
}

void SkyDome::PreDrawUpdate()
{
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

	TerrainMatrixBufferType* mappedBuffer = (TerrainMatrixBufferType*)ms.pData;

	mappedBuffer->world = this->GetModelMatrix();
	mappedBuffer->view = m_moveLookController->ViewMatrix();
	mappedBuffer->projection = m_projectionMatrix;

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);
}

void SkyDome::Update(std::shared_ptr<StepTimer> timer)
{
	DirectX::XMStoreFloat3(&m_position, m_moveLookController->Position());
}