#include "Drawable.h"

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, BasicModelType modelType) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity()),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	PreDrawUpdate([]() {}),
	OnMouseHover([]() {}),
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_nodeName("Unnamed Drawable Node"),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false),
	m_drawWholeBoundingBox(false)
#endif
{
	InitializePipelineConfiguration();

	switch (modelType)
	{
	case BasicModelType::Plane:
		m_mesh = ObjectStore::GetMesh("plane-mesh");
		m_name = "Plane";
		break;
	case BasicModelType::Cube:
		m_mesh = ObjectStore::GetMesh("cube-mesh"); // NOT TESTED
		m_name = "Cube";
		break;
	case BasicModelType::Sphere:
		m_mesh = ObjectStore::GetMesh("sphere-mesh"); // NOT TESTED
		m_name = "Sphere";
		break;
	case BasicModelType::Cylinder:
		m_mesh = ObjectStore::GetMesh("cylinder-mesh"); // NOT TESTED
		m_name = "Cylinder";
		break;
	}
}

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity()),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	PreDrawUpdate([]() {}),
	OnMouseHover([]() {}),
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_nodeName("Unnamed Drawable Node"),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false),
	m_drawWholeBoundingBox(false)
#endif
{
	InitializePipelineConfiguration();

	m_mesh = mesh;
}

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string filename) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity()),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	PreDrawUpdate([]() {}),
	OnMouseHover([]() {}),
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_nodeName("Unnamed Drawable Node"),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false),
	m_drawWholeBoundingBox(false)
#endif
{
	InitializePipelineConfiguration();

	// Getting the stem gets just the filename without the extension
	m_name = std::filesystem::path(filename).stem().string();

	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene == nullptr)
	{
		throw DrawableException(__LINE__, __FILE__, "AssImp ReadFile Error:\n" + std::string(imp.GetErrorString()));
	}

	// When loading a scene/model via assimp, the meshes are just stored in a flat
	// array. The hierarchy of drawables just have an index into that array. So, 
	// for our purpose, the drawable we are building needs to first create a vector
	// of shared pointers to these meshes and then as we build the hierarchy, we
	// assign out the meshes to the corresponding nodes
	std::vector<std::shared_ptr<Mesh>> allMeshes;

	// Build up the vector of all meshes for this model
	std::string meshLookupName;
	for (unsigned int iii = 0; iii < scene->mNumMeshes; ++iii)
	{
		// Create a unique lookup name for the mesh
		meshLookupName = filename + "-" + std::string(scene->mMeshes[iii]->mName.C_Str());

		// If the mesh already exists in ObjectStore, just get it from there
		if (ObjectStore::MeshExists(meshLookupName))
			allMeshes.push_back(ObjectStore::GetMesh(meshLookupName));
		else
		{
			// Mesh does not exist in object store, so load it from assimp and add it to ObjectStore
			LoadMesh(*scene->mMeshes[iii], scene->mMaterials, allMeshes);
			ObjectStore::AddMesh(meshLookupName, allMeshes.back());
		}
	}

	// This constructor is only used for the root node, so just get the root node and if there are any children, a different constructor will be used
	ConstructFromAiNode(*scene->mRootNode, allMeshes, scene->mMaterials);

	// Once the rootNode is created, all meshes will have a BoundingBox, so gather each one and
	// use those values to establish an all encapsulating BoundingBox
	std::vector<XMVECTOR> positions;
	GetBoundingBoxPositionsWithTransformation(DirectX::XMMatrixIdentity(), positions); // Pass identity as parent matrix because root should not be transformed
	m_boundingBox = std::make_unique<BoundingBox>(m_deviceResources, positions);
}

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string name, const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes, const aiMaterial* const* materials) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_projectionMatrix(DirectX::XMMatrixIdentity()),
	m_translation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scaling(XMFLOAT3(1.0f, 1.0f, 1.0f)),
	m_accumulatedModelMatrix(DirectX::XMMatrixIdentity()),
	m_roll(0.0f),
	m_pitch(0.0f),
	m_yaw(0.0f),
	PreDrawUpdate([]() {}),
	OnMouseHover([]() {}),
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name(name),
	m_nodeName("Unnamed Drawable Node"),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false),
	m_drawWholeBoundingBox(false)
#endif
{
	InitializePipelineConfiguration();

	ConstructFromAiNode(node, meshes, materials);
}

void Drawable::InitializePipelineConfiguration()
{
	// XMMatrix to hold the model-view-projection of the previous frame to allow us to test
	// if a model-view-projection constant buffer needs to be updated
	m_previousModelViewProjection = DirectX::XMMatrixIdentity();

	// Use common defaults where possible, otherwise set to nullptr
	SetRasterizerState("solidfill", true);
	SetDepthStencilState("depth-enabled-depth-stencil-state", true);

	m_inputLayout  = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader  = nullptr;

	// Each sampler state array is specific to a different pipeline stage
	for (int iii = (int)SamplerStateBindingLocation::COMPUTE_SHADER; iii <= (int)SamplerStateBindingLocation::PIXEL_SHADER; ++iii)
		m_samplerStateArrays.push_back(nullptr);

	for (int iii = (int)TextureBindingLocation::COMPUTE_SHADER; iii <= (int)TextureBindingLocation::PIXEL_SHADER; ++iii)
		m_textureArrays.push_back(nullptr);

	for (int iii = (int)ConstantBufferBindingLocation::COMPUTE_SHADER; iii <= (int)ConstantBufferBindingLocation::PIXEL_SHADER; ++iii)
		m_constantBufferArrays.push_back(nullptr);
}

void Drawable::ConstructFromAiNode(const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& meshes, const aiMaterial* const* materials)
{
	// Keep track of the node name
	m_nodeName = std::string(node.mName.C_Str());

	// Decompose the transformation to get the individual components
	XMMATRIX transform = DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation))
	);
	XMVECTOR scale, rotation, translation;
	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, transform);

	DirectX::XMStoreFloat3(&m_scaling, scale);
	DirectX::XMStoreFloat3(&m_rotation, rotation);
	DirectX::XMStoreFloat3(&m_translation, translation);

	if (m_rotation.x != 0.0f || m_rotation.y != 0.0f || m_rotation.z != 0.0f)
	{
		std::ostringstream oss;
		oss << "Drawable: '" << m_name << "' Node: '" << m_nodeName << "' has non - unity rotation transform : " << std::endl;
		oss << "   " << m_rotation.x << ", " << m_rotation.y << ", " << m_rotation.z << std::endl;
		oss << "This is not yet supported";
		throw DrawableException(__LINE__, __FILE__, oss.str());
	}

	// Right now, we are forcing each drawable node to only contain a single mesh
	if (node.mNumMeshes > 1)
	{
		std::ostringstream oss;
		oss << "Drawable: '" << m_name << "' Node: '" << m_nodeName << "' has more than one mesh: " << std::endl;
		oss << "   Number of meshes: " << node.mNumMeshes << std::endl;
		oss << "We currently only support a Drawable having at most one mesh";
		throw DrawableException(__LINE__, __FILE__, oss.str());
	}

	// NOTE: the node is NOT required to have a mesh. In the case of OBJ files, 
	// the root node is basically an empty node that houses all the children nodes
	if (node.mNumMeshes == 1)
	{
		m_mesh = meshes[node.mMeshes[0]]; // Reminder: node.mMeshes is just an int array where each int is an index into the all meshes array

		// Extract the material data for the mesh -------------------------------------------------------------------------------

		// Initialize a default Pixel Shader configuration
		PhongPSConfigurationData psConfig;
		psConfig.normalMapEnabled = FALSE; // Use these true/false macros because the underlying BOOL value is a 4-byte boolean
		psConfig.specularMapEnabled = FALSE;
		psConfig.specularIntensity = 0.5f;
		psConfig.specularPower = 1.0f;

		// Get the diffuse texture
		const aiMaterial& material = *materials[m_mesh->GetMaterialIndex()];
		aiString textureFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName);

		// Create the texture
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(m_deviceResources);
		texture->Create(std::string("models/nanosuit-textured/") + textureFileName.C_Str());

		AddTexture(TextureBindingLocation::PIXEL_SHADER, texture, false);

		// Determine if the material has a specular map. If not, just use the shininess value
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> specular = std::make_shared<Texture>(m_deviceResources);
			specular->Create(std::string("models/nanosuit-textured/") + textureFileName.C_Str());

			AddTexture(TextureBindingLocation::PIXEL_SHADER, specular, false);

			psConfig.specularMapEnabled = TRUE;
		}
		else
		{
			// Use the material shininess as the specular power
			material.Get(AI_MATKEY_SHININESS, psConfig.specularPower);
		}

		// For some reason aiTextureType_HEIGHT is used to get the normal maps for OBJ files (not sure about other formats)
		// even though there is an aiTextureType_NORMALS option
		if (material.GetTexture(aiTextureType_HEIGHT, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> normals = std::make_shared<Texture>(m_deviceResources);
			normals->Create(std::string("models/nanosuit-textured/") + textureFileName.C_Str());

			AddTexture(TextureBindingLocation::PIXEL_SHADER, normals, false);

			psConfig.normalMapEnabled = TRUE;
		}

		if (material.GetTexture(aiTextureType_AMBIENT, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_AMBIENT not yet supported");

		if (material.GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_AMBIENT_OCCLUSION not yet supported");

		if (material.GetTexture(aiTextureType_BASE_COLOR, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_BASE_COLOR not yet supported");

		if (material.GetTexture(aiTextureType_CLEARCOAT, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_CLEARCOAT not yet supported");

		if (material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_DIFFUSE_ROUGHNESS not yet supported");

		if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_DISPLACEMENT not yet supported");

		if (material.GetTexture(aiTextureType_EMISSION_COLOR, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_EMISSION_COLOR not yet supported");

		if (material.GetTexture(aiTextureType_EMISSIVE, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_EMISSIVE not yet supported");

		if (material.GetTexture(aiTextureType_LIGHTMAP, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_LIGHTMAP not yet supported");

		if (material.GetTexture(aiTextureType_METALNESS, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_METALNESS not yet supported");

		if (material.GetTexture(aiTextureType_NONE, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_NONE not yet supported");

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_NORMALS not yet supported");

		if (material.GetTexture(aiTextureType_NORMAL_CAMERA, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_NORMAL_CAMERA not yet supported");

		if (material.GetTexture(aiTextureType_OPACITY, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_OPACITY not yet supported");

		if (material.GetTexture(aiTextureType_REFLECTION, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_REFLECTION not yet supported");

		if (material.GetTexture(aiTextureType_SHEEN, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_SHEEN not yet supported");

		if (material.GetTexture(aiTextureType_SHININESS, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_SHININESS not yet supported");

		if (material.GetTexture(aiTextureType_TRANSMISSION, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_TRANSMISSION not yet supported");

		if (material.GetTexture(aiTextureType_UNKNOWN, 0, &textureFileName) == aiReturn_SUCCESS)
			throw DrawableException(__LINE__, __FILE__, "AssImp Texture Type aiTextureType_UNKNOWN not yet supported");
	}

	// Construct the children, must be sure to pass the name that represents the drawable as a whole as well as a reference
	// to the entire list of meshes
	for (unsigned int iii = 0; iii < node.mNumChildren; ++iii)
		m_children.push_back(std::make_unique<Drawable>(m_deviceResources, m_moveLookController, m_name, *node.mChildren[iii], meshes, materials));
}

void Drawable::GetBoundingBoxPositionsWithTransformation(const XMMATRIX& parentModelMatrix, std::vector<XMVECTOR>& positions)
{
	// Right now, we force there to be at most one mesh per drawable
	if (m_mesh != nullptr)
		m_mesh->GetBoundingBoxPositionsWithTransformation(GetPreParentTransformModelMatrix() * parentModelMatrix, positions);
	
	// Get all positions for all children nodes
	for (std::unique_ptr<Drawable>& child : m_children)
		child->GetBoundingBoxPositionsWithTransformation(GetPreParentTransformModelMatrix() * parentModelMatrix, positions);
}

void Drawable::LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials, std::vector<std::shared_ptr<Mesh>>& meshes)
{
	std::vector<OBJVertex> vertices;		// vertices for the vertex buffer
	std::vector<unsigned short> indices;	// indices for the index buffer

	vertices.reserve(mesh.mNumVertices);
	for (unsigned int iii = 0; iii < mesh.mNumVertices; iii++)
	{
		vertices.push_back(
			{
				*reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[iii]),
				*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][iii]), // Use texture at index 0, but there can be >1 texture
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[iii])
			}
		);

		// The V coordinate is stupidly flipped - Not sure how to tell when this is needed
		vertices.back().texture.y = 1 - vertices.back().texture.y;
	}

	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const aiFace& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	meshes.push_back(std::make_shared<Mesh>(m_deviceResources));
	meshes.back()->LoadBuffers<OBJVertex>(vertices, indices);
	meshes.back()->SetMaterialIndex(mesh.mMaterialIndex);
}


void Drawable::SetProjectionMatrix(DirectX::XMMATRIX projection) 
{ 
	m_projectionMatrix = projection; 
	for (std::unique_ptr<Drawable>& child : m_children)
		child->SetProjectionMatrix(projection);
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

void Drawable::SetRasterizerState(std::string lookupName, bool recursive)
{
	// Create the rasterizer state and try adding it to the vector of bindables (NOTE: it will only get added if
	// it was NOT previously added)
	m_rasterizerState = std::dynamic_pointer_cast<RasterizerState>(ObjectStore::GetBindable(lookupName));
	AddBindable(m_rasterizerState);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->SetRasterizerState(lookupName, true);
	}
}

void Drawable::SetDepthStencilState(std::string lookupName, bool recursive)
{
	// Create the depth stencil state and try adding it to the vector of bindables (NOTE: it will only get added if
	// it was NOT previously added)
	m_depthStencilState = std::dynamic_pointer_cast<DepthStencilState>(ObjectStore::GetBindable(lookupName));
	AddBindable(m_depthStencilState);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->SetDepthStencilState(lookupName, true);
	}
}

void Drawable::AddSamplerState(SamplerStateBindingLocation bindingLocation, std::string lookupName, bool recursive)
{
	if (m_samplerStateArrays[(int)bindingLocation] == nullptr)
		m_samplerStateArrays[(int)bindingLocation] = std::make_shared<SamplerStateArray>(m_deviceResources, bindingLocation);

	m_samplerStateArrays[(int)bindingLocation]->AddSamplerState(lookupName);
	AddBindable(m_samplerStateArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddSamplerState(bindingLocation, lookupName, true);
	}
}

void Drawable::AddTexture(TextureBindingLocation bindingLocation, std::shared_ptr<Texture> texture, bool recursive)
{
	if (m_textureArrays[(int)bindingLocation] == nullptr)
		m_textureArrays[(int)bindingLocation] = std::make_shared<TextureArray>(m_deviceResources, bindingLocation);

	m_textureArrays[(int)bindingLocation]->AddTexture(texture);
	AddBindable(m_textureArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddTexture(bindingLocation, texture, true);
	}
}

void Drawable::AddTexture(TextureBindingLocation bindingLocation, std::string lookupName, bool recursive)
{
	if (m_textureArrays[(int)bindingLocation] == nullptr)
		m_textureArrays[(int)bindingLocation] = std::make_shared<TextureArray>(m_deviceResources, bindingLocation);

	m_textureArrays[(int)bindingLocation]->AddTexture(lookupName);
	AddBindable(m_textureArrays[(int)bindingLocation]);

	if (recursive)
	{
		for (std::unique_ptr<Drawable>& child : m_children)
			child->AddTexture(bindingLocation, lookupName, true);
	}
}


void Drawable::AddBindable(std::string lookupName) 
{ 
	AddBindable(ObjectStore::GetBindable(lookupName));
}
void Drawable::AddBindable(std::shared_ptr<Bindable> bindable) 
{ 
	// Only add bindable if it does not already exist
	if (!std::any_of(m_bindables.begin(), m_bindables.end(), [bindable](std::shared_ptr<Bindable> const& b) { return b.get() == bindable.get(); }))
		m_bindables.push_back(bindable);
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

void Drawable::UpdateRenderData()
{
	// This Update function is public and will be called by the Scene. In order to recursively
	// update the drawable hierarchy, we must call the other Update function (which is protected)
	// Because this is the root node in the hierarchy, we can pass in the identity matrix and the
	// next Update function will correctly compute the accumulated model matrix
	UpdateRenderData(DirectX::XMMatrixIdentity());
}

void Drawable::UpdateRenderData(const XMMATRIX& parentModelMatrix)
{
	// Update the model matrix for this node and then update all children
	m_accumulatedModelMatrix = this->GetPreParentTransformModelMatrix() * parentModelMatrix;

	// Update the constant buffers that need updating
	for (std::tuple<std::shared_ptr<ConstantBuffer>, std::function<void(std::shared_ptr<ConstantBuffer>)>> tup : m_updateFunctions)
	{
		// Pass the constant buffer to the update functional
		std::get<1>(tup)(std::get<0>(tup));
	}


	for (std::unique_ptr<Drawable>& child : m_children)
		child->UpdateRenderData(m_accumulatedModelMatrix);
}

void Drawable::UpdateModelViewProjectionBuffer(std::shared_ptr<ConstantBuffer> constantBuffer)
{
	DirectX::XMFLOAT4X4 mvp, prev;
	DirectX::XMStoreFloat4x4(&mvp, m_accumulatedModelMatrix * m_moveLookController->ViewMatrix() * m_moveLookController->ProjectionMatrix());
	DirectX::XMStoreFloat4x4(&prev, m_previousModelViewProjection);	

	if (mvp._11 != prev._11 || mvp._12 != prev._12 || mvp._13 != prev._13 || mvp._14 != prev._14 || 
		mvp._21 != prev._21 || mvp._22 != prev._22 || mvp._23 != prev._23 || mvp._24 != prev._24 ||
		mvp._31 != prev._31 || mvp._32 != prev._32 || mvp._33 != prev._33 || mvp._34 != prev._34 ||
		mvp._41 != prev._41 || mvp._42 != prev._42 || mvp._43 != prev._43 || mvp._44 != prev._44)
	{
		INFOMAN(m_deviceResources);

		ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

		ID3D11Buffer* buffer = constantBuffer->GetRawBufferPointer();

		GFX_THROW_INFO(
			context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
		);

		ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;
		XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * m_moveLookController->ProjectionMatrix();

		DirectX::XMStoreFloat4x4(&(mappedBuffer->model), m_accumulatedModelMatrix);
		DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), m_accumulatedModelMatrix * viewProjection);
		DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m_accumulatedModelMatrix)));

		GFX_THROW_INFO_ONLY(
			context->Unmap(buffer, 0)
		);
	}
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

	// Bind the mesh (vertex and index buffers) 
	// NOTE: Only allowing a single mesh per Drawable at most
	if (m_mesh != nullptr)
	{
		m_mesh->Bind();

		// Update the Model-view-projection constant buffer with the aggregated model matrix
		UpdateModelViewProjectionConstantBuffer();

		// Determine the type of draw call from the mesh
		if (m_mesh->DrawIndexed())
		{
			GFX_THROW_INFO_ONLY(
				m_deviceResources->D3DDeviceContext()->DrawIndexed(m_mesh->IndexCount(), 0u, 0u)
			);
		}
		else
		{
			GFX_THROW_INFO_ONLY(
				m_deviceResources->D3DDeviceContext()->Draw(m_mesh->VertexCount(), 0u)
			);
		}
	}

	// Draw all children
	for (std::unique_ptr<Drawable>& child : m_children)
		child->Draw();

	// Update the previous frame model-view-projection matrix
	m_previousModelViewProjection = m_accumulatedModelMatrix * m_moveLookController->ViewMatrix() * m_moveLookController->ProjectionMatrix();


#ifndef NDEBUG
	// Determine if any bounding boxes need to be draw for any of the nodes
	if (NeedDrawBoundingBox())
	{
		// Bind necessary bindables for drawing box lines

		ObjectStore::GetBindable("solid-vertex-shader")->Bind();				// Vertex Shader
		ObjectStore::GetBindable("solid-vertex-shader-IA")->Bind();				// Input Layout
		ObjectStore::GetBindable("solid-pixel-shader")->Bind();					// Pixel Shader

		ObjectStore::GetBindable("solidfill")->Bind();							// Rasterizer State
		ObjectStore::GetBindable("depth-enabled-depth-stencil-state")->Bind();	// Depth Stencil State

		// Recursively draw any visible bounding boxes
		DrawBoundingBox();
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
	XMMATRIX model = m_accumulatedModelMatrix;
	XMMATRIX viewProjection = m_moveLookController->ViewMatrix() * m_projectionMatrix;
	DirectX::XMStoreFloat4x4(&(mappedBuffer->model), model);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->modelViewProjection), model * viewProjection);
	DirectX::XMStoreFloat4x4(&(mappedBuffer->inverseTransposeModel), DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model)));

	GFX_THROW_INFO_ONLY(
		context->Unmap(vsBuffer.Get(), 0)
	);
}

XMMATRIX Drawable::GetPreParentTransformModelMatrix()
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		GetScaleMatrix() *
		DirectX::XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
}

bool Drawable::IsMouseHovered(float mouseX, float mouseY, float& distance)
{
	// Compute the ray origin and ray direction vector
	XMVECTOR rayOrigin, rayDestination, rayDirection;

	D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	XMMATRIX viewMatrix = m_moveLookController->ViewMatrix();
	XMVECTOR clickPointNear = DirectX::XMVectorSet(mouseX, mouseY, 0.0f, 0.0f);
	XMVECTOR clickPointFar = DirectX::XMVectorSet(mouseX, mouseY, 1.0f, 0.0f);


	// Here, we use the identity matrix for the World matrix because we don't want to translate
	// the vectors as if they were at the origin. If we did want to do that, we would use XMMatrixTranslation(eye.x, eye.y, eye.z)
	rayOrigin = XMVector3Unproject(
		clickPointNear,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		m_projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);		// Use the model matrix for the drawable itself because the bounding box should be offset according to that transformation

	rayDestination = XMVector3Unproject(
		clickPointFar,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		m_projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);

	rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestination, rayOrigin));

	// Determine if the mouse ray intersects the bounding box. If so, call the other IsMouseHovered function to recursively determine if the mouse is over the drawable
	//		- Have to add a check here to make sure the bounding box is not null because we don't yet have a 
	//		- a bounding box for terrain
	// NOTE: We do NOT have to pass the model matrix because each drawable already keeps track of it's accumulated model matrix
	if (m_boundingBox != nullptr && m_boundingBox->RayIntersectionTest(rayOrigin, rayDirection, distance))
		return IsMouseHovered(clickPointNear, clickPointFar, m_projectionMatrix, distance);

	return false;
}

bool Drawable::IsMouseHovered(const XMVECTOR& clickPointNear, const XMVECTOR& clickPointFar, const XMMATRIX& projectionMatrix, float& distance)
{
	// NOTE: We do NOT need to pass the parent's model matrix into this function because this class keeps
	//		 track of the accumulated model matrix. Assuming the Update is done correctly, this class will
	//		 already have an up-to-date model matrix

	// Compute the ray origin and ray direction vector
	XMVECTOR rayOrigin, rayDestination, rayDirection;

	D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	XMMATRIX viewMatrix = m_moveLookController->ViewMatrix();

	// Here, we use the identity matrix for the World matrix because we don't want to translate
	// the vectors as if they were at the origin. If we did want to do that, we would use XMMatrixTranslation(eye.x, eye.y, eye.z)
	rayOrigin = XMVector3Unproject(
		clickPointNear,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);

	rayDestination = XMVector3Unproject(
		clickPointFar,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		m_accumulatedModelMatrix);

	rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestination, rayOrigin));


	// First, run the test for the meshes of this node
	float dist;
	bool found = false;
	distance = FLT_MAX;

	// ModelNode can have multiple meshes, so loop over each one
	if (m_mesh != nullptr && m_mesh->RayIntersectionTest(rayOrigin, rayDirection, dist))
	{
		distance = std::min(distance, dist);
		found = true;
	}

	// Second, run the test for all child nodes
	for (std::unique_ptr<Drawable>& child : m_children)
	{
		if (child->IsMouseHovered(clickPointNear, clickPointFar, projectionMatrix, dist))
		{
			distance = std::min(distance, dist);
			found = true;
		}
	}

	return found;
}



#ifndef NDEBUG
void Drawable::SetMoveLookController(std::shared_ptr<MoveLookController> mlc) 
{ 
	m_moveLookController = mlc; 

	for (std::unique_ptr<Drawable>& child : m_children)
		child->SetMoveLookController(mlc);
}


void Drawable::DrawImGuiCollapsable(std::string id)
{
	if (ImGui::CollapsingHeader((m_name + "##" + id).c_str(), ImGuiTreeNodeFlags_None))
	{
		DrawImGuiDetails(id);
	}
}

void Drawable::DrawImGuiDetails(std::string id)
{
	DrawImGuiPosition(id);
	DrawImGuiRollPitchYaw(id);
	DrawImGuiScale(id);
	DrawImGuiMaterialSettings(id);

	if (m_boundingBox != nullptr)
		ImGui::Checkbox(("Draw Whole Bounding Box##" + id).c_str(), &m_drawWholeBoundingBox);

	DrawImGuiNodeHierarchy(id);
}

void Drawable::DrawImGuiNodeHierarchy(std::string id)
{
	std::string treeNodeName = (m_nodeName == "") ? "Unnamed Node##" + id : m_nodeName + "##" + id;
	if (ImGui::TreeNode(treeNodeName.c_str()))
	{
		if (m_mesh != nullptr)
		{
			if (m_boundingBox != nullptr)
				ImGui::Checkbox(("Draw Bounding Box##" + id).c_str(), &m_drawBoundingBox);

			ImGui::Text("Translation (prior to rotation):");
			ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionX" + id).c_str(), &m_translation.x, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
			ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionY" + id).c_str(), &m_translation.y, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
			ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##modelNodePositionZ" + id).c_str(), &m_translation.z, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);

			ImGui::Text("Orientation:");
			ImGui::Text("   Roll:  "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodeRoll" + m_nodeName + id).c_str(), &m_roll, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
			ImGui::Text("   Pitch: "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodePitch" + m_nodeName + id).c_str(), &m_pitch, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
			ImGui::Text("   Yaw:   "); ImGui::SameLine(); ImGui::SliderFloat(("##modelNodeYaw" + m_nodeName + id).c_str(), &m_yaw, -DirectX::XM_2PI, DirectX::XM_2PI, "%.3f");
		}

		// NOTE: Must reference the unique_ptr (cannot be copied)
		for (std::unique_ptr<Drawable>& child : m_children)
			child->DrawImGuiNodeHierarchy(id);

		ImGui::TreePop();
	}
}

void Drawable::DrawImGuiPosition(std::string id)
{
	ImGui::Text("Position:");
	ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionX" + id).c_str(), &m_translation.x, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
	ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionY" + id).c_str(), &m_translation.y, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
	ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##drawablePositionZ" + id).c_str(), &m_translation.z, 0.05f, -FLT_MAX, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
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
		ImGui::Text("    XYZ: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleXYZ" + id).c_str(), &m_scaling.x, 0.055f, 0.0f, FLT_MAX, "%.01f", ImGuiSliderFlags_None);
		m_scaling.y = m_scaling.x;
		m_scaling.z = m_scaling.x;
	}
	else
	{
		ImGui::Text("    X: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleX" + id).c_str(), &m_scaling.x, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
		ImGui::Text("    Y: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleY" + id).c_str(), &m_scaling.y, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
		ImGui::Text("    Z: "); ImGui::SameLine(); ImGui::DragFloat(("##drawableScaleZ" + id).c_str(), &m_scaling.z, 0.005f, 0.0f, FLT_MAX, "%.001f", ImGuiSliderFlags_None);
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
	if (m_drawBoundingBox || m_drawWholeBoundingBox)
		return true;

	for (std::unique_ptr<Drawable>& child : m_children)
		if (child->NeedDrawBoundingBox())
			return true;

	return false;
}

void Drawable::DrawBoundingBox()
{
	// Draw the bounding box for the drawable as a whole if necessary
	if (m_drawWholeBoundingBox && m_boundingBox != nullptr)
		m_boundingBox->Draw(m_accumulatedModelMatrix, m_moveLookController->ViewMatrix(), m_projectionMatrix);


	// Draw the bounding box for the root mesh if necessary then pass the call to the child nodes
	if (m_drawBoundingBox && m_mesh != nullptr)
		m_mesh->DrawBoundingBox(m_accumulatedModelMatrix, m_moveLookController->ViewMatrix(), m_projectionMatrix);

	for (std::unique_ptr<Drawable>& child : m_children)
		child->DrawBoundingBox();
}
#endif