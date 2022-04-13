#include "SkyDome.h"

SkyDome::SkyDome(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController),
	m_apexColor(XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f)),
	m_centerColor(XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f))
{
	//SetMesh("sky-dome-mesh");
	m_model = std::make_unique<Model>(deviceResources, moveLookController, ObjectStore::GetMesh("sky-dome-mesh"));

	AddBindable("sky-dome-vertex-shader");				// Vertex Shader
	AddBindable("sky-dome-vertex-shader-IA");			// Input Layout
	AddBindable("sky-dome-pixel-shader");				// Pixel Shader
	AddBindable("solidfill"); //"wireframe",			// Rasterizer State
	AddBindable("depth-disabled-depth-stencil-state");	// Depth Stencil State
	//AddBindable("sky-dome-buffers-VS");					// VS Constant buffers
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
	// Pretty much every object will need to submit model/view/projection data to the vertex shader
	// The Scene binds a ModelViewProjectionConstantBuffer object to slot 0 of the vertex shader that
	// can be mapped and written to by any object. The reason we don't automatically perform this update
	// for every drawable is that not every drawable actually requires this update. For example, the Terrain
	// is not a drawable, but instead houses many TerrainCells that are drawable. However, each of these 
	// TerrainCells do not require this update because Terrain is able to set up the model view projection 
	// buffer once before trying to draw each TerrainCell
	UpdateModelViewProjectionConstantBuffer();


	// Updating of any additional constant buffers or other pipeline resources should go here
}

void SkyDome::Update(std::shared_ptr<StepTimer> timer)
{
	DirectX::XMStoreFloat3(&m_position, m_moveLookController->Position());
}




#ifndef NDEBUG
void SkyDome::DrawImGui(std::string id)
{
	if (ImGui::CollapsingHeader(("Sky Dome##" + id).c_str(), ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Do NOT adjust position because the Sky Dome");
		ImGui::Text("is just a sphere around the user.");
		ImGui::Text("Could probably add sliders to adjust color.");
	}
}
#endif