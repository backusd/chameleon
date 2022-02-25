#include "Cube.h"
#include "ObjectStore.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4;

Cube::Cube(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController)
{
	// This must be run first because some of the following methods may use the material/lighting data
	CreateMaterialAndLightData();

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

void Cube::CreateMaterialAndLightData()
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

void Cube::CreateAndAddPSBufferArray()
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
	psConstantBufferArray->AddBuffer("light-properties-buffer");

	m_bindables.push_back(psConstantBufferArray);
}

void Cube::PreDrawUpdate()
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

	/*
	GFX_THROW_INFO_ONLY(
		context->PSGetConstantBuffers(0, 1, psBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(psBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	PhongMaterialProperties* material = (PhongMaterialProperties*)ms.pData;

	material->Material.Emissive = m_material->Material.Emissive;
	material->Material.Ambient = m_material->Material.Ambient;
	material->Material.Diffuse = m_material->Material.Diffuse;
	material->Material.Specular = m_material->Material.Specular;
	material->Material.SpecularPower = m_material->Material.SpecularPower;

	GFX_THROW_INFO_ONLY(
		context->Unmap(psBuffer.Get(), 0)
	);
	*/



	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
	GFX_THROW_INFO_ONLY(
		context->PSGetConstantBuffers(1, 1, psBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(psBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	LightProperties* lp = (LightProperties*)ms.pData;
	lp->EyePosition = m_lightProperties.EyePosition;
	lp->GlobalAmbient = m_lightProperties.GlobalAmbient;
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		lp->Lights[i].Enabled = m_lightProperties.Lights[i].Enabled;
		lp->Lights[i].LightType = m_lightProperties.Lights[i].LightType;
		lp->Lights[i].Color = m_lightProperties.Lights[i].Color;
		lp->Lights[i].SpotAngle = m_lightProperties.Lights[i].SpotAngle;
		lp->Lights[i].ConstantAttenuation = m_lightProperties.Lights[i].ConstantAttenuation;
		lp->Lights[i].LinearAttenuation = m_lightProperties.Lights[i].LinearAttenuation;
		lp->Lights[i].QuadraticAttenuation = m_lightProperties.Lights[i].QuadraticAttenuation;
		lp->Lights[i].Position = m_lightProperties.Lights[i].Position;
		lp->Lights[i].Direction = m_lightProperties.Lights[i].Direction;
	}

	GFX_THROW_INFO_ONLY(
		context->Unmap(psBuffer.Get(), 0)
	);
}

DirectX::XMMATRIX Cube::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_xSideLength, m_ySideLength, m_zSideLength);
}

void Cube::Update(std::shared_ptr<StepTimer> timer)
{

}