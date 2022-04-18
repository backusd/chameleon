#include "Nanosuit.h"
#include "Terrain.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

Nanosuit::Nanosuit(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController),
	m_scaleFactor(0.5f),
	m_movementSpeed(10.0f),
	m_movingForward(false),
	m_currentTime(0.0),
	m_previousTime(0.0),
	m_currentTerrain(nullptr),
	m_movingToClickLocation(false),
	m_clickLocation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_velocityVector(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_startTime(0.0),
	m_endTime(0.0)
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
	// If the terrain has changed, update it first
	if (m_currentTerrain != terrain)
		m_currentTerrain = terrain;

	m_currentTime = timer->GetTotalSeconds();
	double timeDelta = m_currentTime - m_previousTime;
	
	if (m_movingForward)
	{
		float deltaX = static_cast<float>(m_movementSpeed * sin(m_yaw) * timeDelta);
		m_position.x += deltaX;

		float deltaZ = static_cast<float>(m_movementSpeed * cos(m_yaw) * timeDelta);
		m_position.z += deltaZ;


		// stop automated move

	}
	else if (m_movingToClickLocation)
	{
		if (m_currentTime > m_endTime)
		{
			m_position = m_clickLocation;
			m_movingToClickLocation = false;
		}
		else
		{
			m_position.x += m_velocityVector.x * timeDelta;
			m_position.y += m_velocityVector.y * timeDelta;
			m_position.z += m_velocityVector.z * timeDelta;
		}
	}

	// Update the y position according to the terrain height
	// NOTE: For game logic, it might make sense to only update the height if
	// the player is actuall moving forward. However, getting the terrain height
	// SHOULD be constant time lookup and it helps with debugging to just make
	// sure it is always set.
	m_position.y = m_currentTerrain->GetHeight(m_position.x, m_position.z);

	m_previousTime = m_currentTime;
}

XMFLOAT3 Nanosuit::CenterOfModel()
{
	// Top of the nanosuit is about 15.4 units, so halfway up is about 7.7
	// Scale this down by some scale factor
	return XMFLOAT3(m_position.x, m_position.y + (7.7f * m_scaleFactor), m_position.z);
}

void Nanosuit::MoveTo(DirectX::XMFLOAT3 location, float speed)
{
	// The actual movement will take place in the Update function, so we must
	// set certain parameters here for the movement to be triggered
	m_movingToClickLocation = true;	// Let the Update function know we are actively moving
	m_clickLocation = location;		// Set the destination location
	m_startTime = m_currentTime;	// Set the time to the time that was computed in the last Update

	// Compute the direction between start and finish
	XMFLOAT3 direction;
	direction.x = m_clickLocation.x - m_position.x;
	direction.y = m_clickLocation.y - m_position.y;
	direction.z = m_clickLocation.z - m_position.z;

	// Compute the expected end time for the movement
	XMFLOAT3 length;
	DirectX::XMStoreFloat3(&length, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&direction)));
	m_endTime = m_startTime + (length.x / speed);

	// Normalize the direction and scale by the speed to compute the velocity vector
	DirectX::XMStoreFloat3(&m_velocityVector, 
		DirectX::XMVectorScale(
			DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)), 
			speed
		)
	);
}

void Nanosuit::LookLeft(float angle)
{
	// don't let the angle get arbitrarily high, so wrap between [-PI, PI]
	m_yaw = fmod(m_yaw + angle, DirectX::XM_2PI);
}

void Nanosuit::LookRight(float angle)
{
	// don't let the angle get arbitrarily high, so wrap between [-PI, PI]
	m_yaw = fmod(m_yaw - angle, DirectX::XM_2PI);
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

void Nanosuit::DrawImGuiPosition(std::string id)
{
	ImGui::Text("Position:");
	ImGui::Text("    X: "); ImGui::SameLine(); ImGui::SliderFloat(("##drawablePositionX" + id).c_str(), &m_position.x, m_currentTerrain->GetMinX(), m_currentTerrain->GetMaxX(), "%.3f");
	ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::SliderFloat(("##drawablePositionZ" + id).c_str(), &m_position.z, m_currentTerrain->GetMinZ(), m_currentTerrain->GetMaxZ(), "%.3f");
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