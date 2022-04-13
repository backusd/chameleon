#include "Nanosuit.h"

using DirectX::XMFLOAT4;

Nanosuit::Nanosuit(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController)
{
	// This must be run first because some of the following methods may use the material data
	CreateMaterialData();

	m_model = std::make_unique<Model>(deviceResources, moveLookController, "models/nanosuit.obj");

	AddBindable("phong-texture-vertex-shader");			// Vertex Shader
	AddBindable("phong-texture-vertex-shader-IA");		// Input Layout
	AddBindable("phong-pixel-shader");					// Pixel Shader
	AddBindable("solidfill"); //wireframe/solidfill 	// Rasterizer State
	AddBindable("depth-enabled-depth-stencil-state");	// Depth Stencil State

	// Function to create the PS constant buffer array - it will create an immutable constant buffer to hold material data
	CreateAndAddPSBufferArray();
}

void Nanosuit::CreateMaterialData()
{
	m_material = new PhongMaterialProperties();
	m_material->Material.Emissive = XMFLOAT4(0.091f, 0.091f, 0.091f, 1.0f);
	m_material->Material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_material->Material.Diffuse = XMFLOAT4(0.197f, 0.197f, 0.197f, 1.0f);
	m_material->Material.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material->Material.SpecularPower = 7.0f;
}

void Nanosuit::CreateAndAddPSBufferArray()
{
	// Create an immutable constant buffer and load it with the material data
	m_materialConstantBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	m_materialConstantBuffer->CreateBuffer<PhongMaterialProperties>(
		D3D11_USAGE_DEFAULT,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(m_material)	// Initial Data: Fill the buffer with material data
		);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);

	// Add the material constant buffer and the lighting constant buffer
	psConstantBufferArray->AddBuffer(m_materialConstantBuffer);
	m_bindables.push_back(psConstantBufferArray);
}

void Nanosuit::PreDrawUpdate()
{
	INFOMAN(m_deviceResources);



	// If in debug mode, update the material properties if a change has been made
#ifndef NDEBUG
	if (m_materialNeedsUpdate)
	{
		m_material->Material.Emissive = XMFLOAT4(m_emmissive);
		m_material->Material.Ambient = XMFLOAT4(m_ambient);
		m_material->Material.Diffuse = XMFLOAT4(m_diffuse);
		m_material->Material.Specular = XMFLOAT4(m_specular);
		m_material->Material.SpecularPower = m_specularPower;

		GFX_THROW_INFO_ONLY(
			m_deviceResources->D3DDeviceContext()->UpdateSubresource(
				m_materialConstantBuffer->GetRawBufferPointer(),
				0, nullptr,
				static_cast<void*>(m_material),
				0, 0)
		);

		m_materialNeedsUpdate = false;
	}
#endif
}

void Nanosuit::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain)
{
	// 
}



#ifndef NDEBUG
void Nanosuit::DrawImGui(std::string id)
{
	if (ImGui::CollapsingHeader(("Nanosuit_" + id).c_str(), ImGuiTreeNodeFlags_None))
	{
		DrawImGuiPosition(id);
		DrawImGuiRollPitchYaw(id);
		DrawImGuiMaterialSettings(id);
		m_model->DrawImGui(id);
	}
}

void Nanosuit::DrawImGuiMaterialSettings(std::string id)
{
	if (ImGui::TreeNode(("Material##Nanosuit_" + id).c_str()))
	{
		if (ImGui::SliderFloat4(("Emmissive##Nanosuit_" + id).c_str(), m_emmissive, 0.0f, 1.0f))
			m_materialNeedsUpdate = true;

		if(ImGui::SliderFloat4(("Ambient##Nanosuit_" + id).c_str(), m_ambient, 0.0f, 1.0f))
			m_materialNeedsUpdate = true;

		if(ImGui::SliderFloat4(("Diffuse##Nanosuit_" + id).c_str(), m_diffuse, 0.0f, 1.0f))
			m_materialNeedsUpdate = true;

		if(ImGui::SliderFloat4(("Specular##Nanosuit_" + id).c_str(), m_specular, 0.0f, 1.0f))
			m_materialNeedsUpdate = true;

		if(ImGui::SliderFloat(("Specular Power##Nanosuit_" + id).c_str(), &m_specularPower, 1.0f, 128.f, "%.1f"))
			m_materialNeedsUpdate = true;

		ImGui::TreePop();
	}
}
#endif