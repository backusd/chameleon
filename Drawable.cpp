#include "Drawable.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_scaleX(1.0f),
	m_scaleY(1.0f),
	m_scaleZ(1.0f),
	PreDrawUpdate([]() {}),
	OnMouseHover([]() {}),
	OnMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable")
{
}

void Drawable::SetModel(std::string fileName) {
	m_model = std::make_unique<Model>(m_deviceResources, m_moveLookController, fileName);

	// Getting the stem gets just the filename without the extension
	m_name = std::filesystem::path(fileName).stem().string();
}	

void Drawable::SetPhongMaterial(std::unique_ptr<PhongMaterialProperties> material)
{ 
	m_material = std::move(material);

#ifndef NDEBUG
	m_materialNeedsUpdate = true;

	m_emmissive[0] = m_material->Material.Emissive.x;
	m_emmissive[1] = m_material->Material.Emissive.y;
	m_emmissive[2] = m_material->Material.Emissive.z;
	m_emmissive[3] = m_material->Material.Emissive.w;

	m_ambient[0] = m_material->Material.Ambient.x;
	m_ambient[1] = m_material->Material.Ambient.y;
	m_ambient[2] = m_material->Material.Ambient.z;
	m_ambient[3] = m_material->Material.Ambient.w;

	m_diffuse[0] = m_material->Material.Diffuse.x;
	m_diffuse[1] = m_material->Material.Diffuse.y;
	m_diffuse[2] = m_material->Material.Diffuse.z;
	m_diffuse[3] = m_material->Material.Diffuse.w;

	m_specular[0] = m_material->Material.Specular.x;
	m_specular[1] = m_material->Material.Specular.y;
	m_specular[2] = m_material->Material.Specular.z;
	m_specular[3] = m_material->Material.Specular.w;

	m_specularPower = m_material->Material.SpecularPower;
#endif
}


void Drawable::CreateAndAddPSBufferArray()
{
	// Create an immutable constant buffer and load it with the material data
	m_materialConstantBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	m_materialConstantBuffer->CreateBuffer<PhongMaterialProperties>(
		D3D11_USAGE_DEFAULT,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(m_material.get())	// Initial Data: Fill the buffer with material data
		);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);

	// Add the material constant buffer and the lighting constant buffer
	psConstantBufferArray->AddBuffer(m_materialConstantBuffer);
	m_bindables.push_back(psConstantBufferArray);
}

void Drawable::Draw()
{
	INFOMAN(m_deviceResources);

	// Bind all bindables and then draw the model
	for (std::shared_ptr<Bindable> bindable : m_bindables)
		bindable->Bind();

	// The PreDrawUpdate function will execute immediately prior to performing the actual Draw call. 
	// This allows updating of any constant buffers or other necessary updates that need to take place
	// before submitting the vertices to be rendered
	PreDrawUpdate();

	m_model->Draw(GetModelMatrix(), m_projectionMatrix);

#ifndef NDEBUG
	// Draw bounding boxes one or more meshes within the model need to have their bounding box drawn
	if (m_model->NeedDrawBoundingBox())
	{
		// Bind necessary bindables for drawing box lines

		ObjectStore::GetBindable("solid-vertex-shader")->Bind();				// Vertex Shader
		ObjectStore::GetBindable("solid-vertex-shader-IA")->Bind();				// Input Layout
		ObjectStore::GetBindable("solid-pixel-shader")->Bind();					// Pixel Shader

		ObjectStore::GetBindable("solidfill")->Bind();							// Rasterizer State
		ObjectStore::GetBindable("depth-enabled-depth-stencil-state")->Bind();	// Depth Stencil State


		// Issue draw call to the model
		m_model->DrawBoundingBox(GetModelMatrix(), m_projectionMatrix);
	}
#endif
}



void Drawable::UpdateModelViewProjectionConstantBuffer()
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
}

XMMATRIX Drawable::GetModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		GetScaleMatrix() *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
}

bool Drawable::IsMouseHovered(float mouseX, float mouseY, float& distance)
{
	// Must pass the model matrix because it will be used when unprojecting
	// the mouse ray
	return m_model->IsMouseHovered(mouseX, mouseY, GetModelMatrix(), m_projectionMatrix, distance);
}

#ifndef NDEBUG
void Drawable::DrawImGui(std::string id)
{
	if (ImGui::CollapsingHeader((m_name + "##" + id).c_str(), ImGuiTreeNodeFlags_None))
	{
		DrawImGuiPosition(id);
		DrawImGuiRollPitchYaw(id);
		DrawImGuiScale(id);
		DrawImGuiMaterialSettings(id);
		m_model->DrawImGui(id);
	}
}

void Drawable::DrawImGuiPosition(std::string id)
{
	ImGui::Text("Position:");
	ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionX" + id).c_str(), &m_position.x, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
	ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionY" + id).c_str(), &m_position.y, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
	ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionZ" + id).c_str(), &m_position.z, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
}

void Drawable::DrawImGuiRollPitchYaw(std::string id)
{
	ImGui::Text("Orientation:");
	ImGui::Text("   Roll:  "); ImGui::SameLine(); ImGui::SliderFloat(("##drawableRoll" + id).c_str(), &m_roll, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
	ImGui::Text("   Pitch: "); ImGui::SameLine(); ImGui::SliderFloat(("##drawablePitch" + id).c_str(), &m_pitch, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
	ImGui::Text("   Yaw:   "); ImGui::SameLine(); ImGui::SliderFloat(("##drawableYaw" + id).c_str(), &m_yaw, -DirectX::XM_PI, DirectX::XM_PI, "%.3f");
}

void Drawable::DrawImGuiScale(std::string id)
{
	ImGui::Text("Scale:"); 
	ImGui::SameLine(); 
	ImGui::Checkbox("Sync values", &m_syncScaleValues);

	if (m_syncScaleValues)
	{
		ImGui::Text("    XYZ: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleXYZ" + id).c_str(), &m_scaleX, 0.055f, 0.0f, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
		m_scaleY = m_scaleX;
		m_scaleZ = m_scaleX;
	}
	else
	{
		ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleX" + id).c_str(), &m_scaleX, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
		ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleY" + id).c_str(), &m_scaleY, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
		ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleZ" + id).c_str(), &m_scaleZ, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
	}
}

void Drawable::DrawImGuiMaterialSettings(std::string id)
{
	if (m_material == nullptr)
	{
		ImGui::Text("No material data");
	}
	else
	{
		if (ImGui::TreeNode(("Material##Nanosuit_" + id).c_str()))
		{
			if (ImGui::SliderFloat4(("Emmissive##Nanosuit_" + id).c_str(), m_emmissive, 0.0f, 1.0f))
				m_materialNeedsUpdate = true;

			if (ImGui::SliderFloat4(("Ambient##Nanosuit_" + id).c_str(), m_ambient, 0.0f, 1.0f))
				m_materialNeedsUpdate = true;

			if (ImGui::SliderFloat4(("Diffuse##Nanosuit_" + id).c_str(), m_diffuse, 0.0f, 1.0f))
				m_materialNeedsUpdate = true;

			if (ImGui::SliderFloat4(("Specular##Nanosuit_" + id).c_str(), m_specular, 0.0f, 1.0f))
				m_materialNeedsUpdate = true;

			if (ImGui::SliderFloat(("Specular Power##Nanosuit_" + id).c_str(), &m_specularPower, 1.0f, 128.f, "%.1f"))
				m_materialNeedsUpdate = true;

			ImGui::TreePop();
		}
	}
}

void Drawable::UpdatePhongMaterial()
{
	INFOMAN(m_deviceResources);

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
				static_cast<void*>(m_material.get()),
				0, 0)
		);

		m_materialNeedsUpdate = false;
	}
}

bool Drawable::NeedDrawBoundingBox()
{
	return m_model->NeedDrawBoundingBox();
}
#endif