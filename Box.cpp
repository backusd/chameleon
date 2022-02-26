#include "Box.h"
#include "ObjectStore.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4;

Box::Box(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController)
{
	// This must be run first because some of the following methods may use the material data
	CreateMaterialData();

	SetMesh("box-filled-mesh");

	AddBindable("phong-vertex-shader");					// Vertex Shader
	AddBindable("phong-vertex-shader-IA");				// Input Layout
	AddBindable("phong-pixel-shader");					// Pixel Shader
	AddBindable("solidfill"); //"wireframe",			// Rasterizer State
	AddBindable("depth-enabled-depth-stencil-state");	// Depth Stencil State
	AddBindable("cube-buffers-VS");						// VS Constant buffers
	// AddBindable("cube-buffers-PS");						// PS Constant buffers

	// Function to create the PS constant buffer array - it will create an immutable constant buffer to hold material data
	CreateAndAddPSBufferArray();
}

void Box::CreateMaterialData()
{
	m_material = new PhongMaterialProperties();
	m_material->Material.Emissive = XMFLOAT4(0.4f, 0.14f, 0.14f, 1.0f);
	m_material->Material.Ambient = XMFLOAT4(1.0f, 0.75f, 0.75f, 1.0f);
	m_material->Material.Diffuse = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
	m_material->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_material->Material.SpecularPower = 6.0f;
}

void Box::CreateAndAddPSBufferArray()
{
	// Create an immutable constant buffer and load it with the material data
	std::shared_ptr<ConstantBuffer> materialBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	materialBuffer->CreateBuffer<PhongMaterialProperties>(
		D3D11_USAGE_IMMUTABLE,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(m_material)	// Initial Data: Fill the buffer with material data
	);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);
	
	// Add the material constant buffer and the lighting constant buffer
	psConstantBufferArray->AddBuffer(materialBuffer);
	m_bindables.push_back(psConstantBufferArray);
}

void Box::PreDrawUpdate()
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

	ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;

	XMMATRIX model = this->GetModelMatrix();
	XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * m_projectionMatrix;
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), model);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), model * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);


	// Update PS constant buffer with material and light data
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	Microsoft::WRL::ComPtr<ID3D11Buffer> psBuffer;
}

DirectX::XMMATRIX Box::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_xSideLength, m_ySideLength, m_zSideLength);
}

void Box::Update(std::shared_ptr<StepTimer> timer)
{

}