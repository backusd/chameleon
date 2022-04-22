#include "Box.h"
#include "ObjectStore.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT4;

Box::Box(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController)
{
	// This must be run first because some of the following methods may use the material data
	CreateMaterialData();

	//SetMesh("box-filled-mesh");
	m_model = std::make_unique<Model>(deviceResources, moveLookController, ObjectStore::GetMesh("box-filled-mesh"));

	AddBindable("phong-vertex-shader");					// Vertex Shader
	AddBindable("phong-vertex-shader-IA");				// Input Layout
	AddBindable("phong-pixel-shader");					// Pixel Shader
	AddBindable("solidfill"); //"wireframe",			// Rasterizer State
	AddBindable("depth-enabled-depth-stencil-state");	// Depth Stencil State
	// AddBindable("cube-buffers-VS");						// VS Constant buffers
	// AddBindable("cube-buffers-PS");						// PS Constant buffers

	// Function to create the PS constant buffer array - it will create an immutable constant buffer to hold material data
	CreateAndAddPSBufferArray();

	PreDrawUpdate = [this]() {
		// Pretty much every object will need to submit model/view/projection data to the vertex shader
		// The Scene binds a ModelViewProjectionConstantBuffer object to slot 0 of the vertex shader that
		// can be mapped and written to by any object. The reason we don't automatically perform this update
		// for every drawable is that not every drawable actually requires this update. For example, the Terrain
		// is not a drawable, but instead houses many TerrainCells that are drawable. However, each of these 
		// TerrainCells do not require this update because Terrain is able to set up the model view projection 
		// buffer once before trying to draw each TerrainCell
		UpdateModelViewProjectionConstantBuffer();


		// Updating of any additional constant buffers or other pipeline resources should go here
	};
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

DirectX::XMMATRIX Box::GetScaleMatrix()
{
	return DirectX::XMMatrixScaling(m_xSideLength, m_ySideLength, m_zSideLength);
}

void Box::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain)
{

}