#include "Lighting.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMVECTORF32;
using DirectX::XMVECTOR;

Lighting::Lighting(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController, ObjectStore::GetMesh("sphere-mesh"))
{
	m_translation = XMFLOAT3(0.0f, 5.0f, 10.0f);

	// This must be run first because some of the following methods may use the lighting data
	CreateLightProperties();

	AddBindable("phong-vertex-shader");					// Vertex Shader
	AddBindable("phong-vertex-shader-IA");				// Input Layout
	AddBindable("phong-pixel-shader");					// Pixel Shader

	//SetMesh("solid-sphere-mesh");
	//AddBindable("solid-vertex-shader");					// Vertex Shader
	//AddBindable("solid-vertex-shader-IA");				// Input Layout
	//AddBindable("solid-pixel-shader");					// Pixel Shader

	AddBindable("solidfill"); //wireframe/solidfill 	// Rasterizer State
	AddBindable("depth-enabled-depth-stencil-state");	// Depth Stencil State

	// Function to create the PS constant buffer array - it will create an immutable constant buffer to hold material data
	CreateAndBindLightPropertiesBuffer();


	// ONLY needed if doing Phong shading
	CreateAndAddPSBufferArray();

	PreDrawUpdate = [this]() {

		// Update the light location as well as the eye position of the camera
		DirectX::XMStoreFloat4(&m_lightProperties.EyePosition, m_moveLookController->Position());
		m_lightProperties.Lights[0].Position = XMFLOAT4(m_translation.x, m_translation.y, m_translation.z, 1.0f);

		// Updating of any additional constant buffers or other pipeline resources should go here
		UpdatePSConstantBuffer();
	};
}

void Lighting::CreateLightProperties()
{
	m_lightProperties = LightProperties();
	m_lightProperties.GlobalAmbient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	// The initial eye position - you will want to modify MoveLookController so the Eye
	// position can be retrieved to also update the light position
	//m_lightProperties.EyePosition = XMFLOAT4(0.0f, 0.0f, -2.0f, 0.0f);
	DirectX::XMStoreFloat4(&m_lightProperties.EyePosition, m_moveLookController->Position());

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
		
		XMFLOAT4 LightPosition = XMFLOAT4(m_translation.x, m_translation.y, m_translation.z, 1.0f);
		light.Position = LightPosition;
		XMVECTOR LightDirection = DirectX::XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
		XMStoreFloat4(&light.Direction, DirectX::XMVector3Normalize(LightDirection));

		m_lightProperties.Lights[i] = light;
	}
}

void Lighting::CreateAndBindLightPropertiesBuffer()
{
	// The scene will be responsible for keeping track of all lights in the scene
	// To reduce the number of times that the lighting data gets bound to the pipeline,
	// the scene can simply bind the lighting data constant buffer once at program launch
	// and then only update the buffer when necessary. ALL PS shader programs will need to 
	// be aware that the lighting constant buffer is bound to slot 0 and they should bind
	// additional buffers starting at slot 1

	INFOMAN(m_deviceResources);

	m_lightConstantBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);

	// For some reason, the lighting flickers when updating the buffers using map/unmap
	// So instead, we currently update the light data in the PS constant buffer using UpdateSubresource,
	// which requires D3D11_USAGE_DEFAULT (see UpdatePSConstantBuffer)

	/*
	m_lightConstantBuffer->CreateBuffer<LightProperties>(
		D3D11_USAGE_DYNAMIC,			// Usage: Default - will not be CPU writeable
		D3D11_CPU_ACCESS_WRITE,			// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&m_lightProperties)	// Initial Data: Fill the buffer with light data
		);
	*/
	
	m_lightConstantBuffer->CreateBuffer<LightProperties>(
		D3D11_USAGE_DEFAULT,			// Usage: Default - will not be CPU writeable
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&m_lightProperties)	// Initial Data: Fill the buffer with light data
		);
	
	Activate();
	//ID3D11Buffer* buffer[1] = { m_lightConstantBuffer->GetRawBufferPointer() };
	//GFX_THROW_INFO_ONLY(
	//	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 1u, buffer)
	//);
}

void Lighting::Activate()
{
	INFOMAN(m_deviceResources);

	// When activating a new scene, the lighting must be activated
	// This just means the light constant buffer must be bound
	ID3D11Buffer* buffer[1] = { m_lightConstantBuffer->GetRawBufferPointer() };
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 1u, buffer)
	);
}

void Lighting::CreateAndAddPSBufferArray()
{
	m_material = new PhongMaterialProperties();
	m_material->Material.Emissive = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material->Material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material->Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material->Material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material->Material.SpecularPower = 6.0f;

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


void Lighting::UpdatePSConstantBuffer()
{
	INFOMAN(m_deviceResources);

	// For some reason, the lighting flickers when updating the buffers using map/unmap
	// So instead, we currently update the light data in the PS constant buffer using UpdateSubresource,
	// which requires D3D11_USAGE_DEFAULT (see CreateAndBindLightPropertiesBuffer)

	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->UpdateSubresource(
			m_lightConstantBuffer->GetRawBufferPointer(),
			0, nullptr,
			static_cast<void*>(&m_lightProperties),
			0, 0)
	);


	/*
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Update PS constant buffer with lighting info
	Microsoft::WRL::ComPtr<ID3D11Buffer> psBuffer;
	GFX_THROW_INFO_ONLY(
		context->PSGetConstantBuffers(0, 1, psBuffer.ReleaseAndGetAddressOf())
	);

	GFX_THROW_INFO(
		context->Map(psBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
	);

	LightProperties* mappedBuffer = (LightProperties*)ms.pData;
	
	// Update the eye position and the first light position
	DirectX::XMStoreFloat4(&mappedBuffer->EyePosition, m_moveLookController->Position());
	mappedBuffer->Lights[0].Position = m_light1Position;

	GFX_THROW_INFO_ONLY(
		context->Unmap(psBuffer.Get(), 0)
	);
	*/
}



#ifndef NDEBUG
void Lighting::DrawImGuiCollapsable(std::string id)
{
	if (ImGui::CollapsingHeader(("Lighting##" + id).c_str(), ImGuiTreeNodeFlags_None))
	{
		DrawImGuiDetails(id);
	}
}

void Lighting::DrawImGuiDetails(std::string id)
{
	ImGui::Text("Position:");

	ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##lightPositionX" + id).c_str(), &m_translation.x, 0.5f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_None);
	ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##lightPositionY" + id).c_str(), &m_translation.y, 0.5f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_None);
	ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##lightPositionZ" + id).c_str(), &m_translation.z, 0.5f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_None);

	ImGui::Text("");
	ImGui::Text("Could make this an array of lights and be able to change");
	ImGui::Text("color and light type");
}
#endif
