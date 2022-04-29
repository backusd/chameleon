#include "Model.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	,m_drawBoundingBox(false)
#endif
{
	// Create the root node - will initially have no data and no name
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);
	m_rootNode->AddMesh(mesh);
}

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, BasicModelType basicModelType) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_rootNode(nullptr),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
{
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);

	switch (basicModelType)
	{
	case BasicModelType::Plane:
		m_rootNode->AddMesh(ObjectStore::GetMesh("plane-mesh"));
		break;
	case BasicModelType::Cube:
		m_rootNode->AddMesh(ObjectStore::GetMesh("cube-mesh")); // NOT TESTED
		break;
	case BasicModelType::Sphere:
		m_rootNode->AddMesh(ObjectStore::GetMesh("sphere-mesh"));  // NOT TESTED
		break;
	case BasicModelType::Cylinder:
		m_rootNode->AddMesh(ObjectStore::GetMesh("cylinder-mesh"));  // NOT TESTED
		break;
	}
}

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string fileName) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_rootNode(nullptr),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	,m_drawBoundingBox(false)
#endif
{
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(fileName.c_str(),	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, "AssImp ReadFile Error:\n" + std::string(imp.GetErrorString()));
	}

	// Build up the vector of all meshes for this model
	std::string meshLookupName;
	for (unsigned int iii = 0; iii < scene->mNumMeshes; ++iii)
	{
		// Create a unique lookup name for the mesh
		meshLookupName = fileName + "-" + std::string(scene->mMeshes[iii]->mName.C_Str());

		// If the mesh already exists in ObjectStore, just get it from there
		if (ObjectStore::MeshExists(meshLookupName))
			m_meshes.push_back(ObjectStore::GetMesh(meshLookupName));
		else
		{
			// Mesh does not exist in object store, so load it from assimp and add it to ObjectStore
			LoadMesh(*scene->mMeshes[iii], scene->mMaterials);
			ObjectStore::AddMesh(meshLookupName, m_meshes.back());
		}
	}

	// Build up the node hierarchy (just requires constructing the root node, which will 
	// recursively build up the children nodes)
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController, *scene->mRootNode, m_meshes);

	// Once the rootNode is created, all meshes will have a BoundingBox, so gather each one and
	// use those values to establish an all encapsulating BoundingBox
	std::vector<XMVECTOR> positions;
	m_rootNode->GetBoundingBoxPositionsWithTransformation(DirectX::XMMatrixIdentity(), positions); // Pass identity as parent matrix because root should not be transformed
	m_boundingBox = std::make_unique<BoundingBox>(m_deviceResources, positions);
}

void Model::LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials)
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

	m_meshes.push_back(std::make_shared<Mesh>(m_deviceResources));
	m_meshes.back()->LoadBuffers<OBJVertex>(vertices, indices);

	// Load the material ==================================================================================================================
	
	// Initialize a default Pixel Shader configuration
	PhongPSConfigurationData psConfig;
	psConfig.normalMapEnabled = FALSE; // Use these true/false macros because the underlying BOOL value is a 4-byte boolean
	psConfig.specularMapEnabled = FALSE;
	psConfig.specularIntensity = 0.5f;
	psConfig.specularPower = 1.0f;

	// Material index will be negative if there is no material for this 
	if (mesh.mMaterialIndex >= 0)
	{
		// Get the diffuse texture
		const aiMaterial& material = *materials[mesh.mMaterialIndex];
		aiString textureFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName);
		
		// Create the texture
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(m_deviceResources);
		texture->Create(std::string("models/nanosuit-textured/") + textureFileName.C_Str());

		// Add the texture to a texture array (texture itself is not bindable)
		std::shared_ptr<TextureArray> textureArray = std::make_shared<TextureArray>(m_deviceResources, TextureBindingLocation::PIXEL_SHADER);
		textureArray->AddTexture(texture);


		// Determine if the material has a specular map. If not, just use the shininess value
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			std::shared_ptr<Texture> specular = std::make_shared<Texture>(m_deviceResources);
			specular->Create(std::string("models/nanosuit-textured/") + textureFileName.C_Str());
			textureArray->AddTexture(specular);

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
			textureArray->AddTexture(normals);

			psConfig.normalMapEnabled = TRUE;
		}



		if (material.GetTexture(aiTextureType_AMBIENT, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_BASE_COLOR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_CLEARCOAT, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_DISPLACEMENT, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_EMISSION_COLOR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_EMISSIVE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_LIGHTMAP, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_METALNESS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_NONE, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_NORMAL_CAMERA, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_OPACITY, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_REFLECTION, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_SHEEN, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_SHININESS, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_TRANSMISSION, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}

		if (material.GetTexture(aiTextureType_UNKNOWN, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			int iii = 0;
		}


		m_meshes.back()->AddBindable(textureArray);
		m_meshes.back()->AddBindable(std::make_shared<SamplerState>(m_deviceResources));
	}


	// Create a PS constant buffer for the PS configuration data
	std::shared_ptr<ConstantBuffer> specularBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
	specularBuffer->CreateBuffer<PhongPSConfigurationData>(
		D3D11_USAGE_DEFAULT,			// Usage: Read-only by the GPU. Not accessible via CPU. MUST be initialized at buffer creation
		0,								// CPU Access: No CPU access
		0,								// Misc Flags: No miscellaneous flags
		0,								// Structured Byte Stride: Not totally sure, but I don't think this needs to be set because even though it is a structured buffer, there is only a single element
		static_cast<void*>(&psConfig)	// Initial Data: Fill the buffer with config data
		);

	// Create a constant buffer array which will be added as a bindable
	std::shared_ptr<ConstantBufferArray> psConstantBufferArray = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);

	// Add the material constant buffer and the lighting constant buffer
	psConstantBufferArray->AddBuffer(specularBuffer);
	m_meshes.back()->AddBindable(psConstantBufferArray);
}

void Model::Update(const XMMATRIX& parentModelMatrix)
{
	// Update the model matrix for each node in the hierarchy
	m_rootNode->Update(parentModelMatrix);
}

void Model::Draw(const XMMATRIX& projectionMatrix)
{
	// Draw the root node applying no transformations yet
	m_rootNode->Draw(projectionMatrix);
}

bool Model::IsMouseHovered(float mouseX, float mouseY, const XMMATRIX& modelMatrix, const XMMATRIX& projectionMatrix, float& distance)
{
	// NOTE: We DO need to pass the Drawable's model matrix into this function because this class does not keep
	//		 track of a model matrix. However, the ModelNodes do keep track of their accumulated matrix, therefore,
	//		 we do NOT need to pass this model matrix to the root node when calling m_rootNode->IsMouseHovered(). 
	//		 Assuming the Update is done correctly, the root node will already have an up-to-date model matrix


	// Compute the ray origin and ray direction vector
	XMVECTOR rayOrigin, rayDestination, rayDirection;

	D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	XMMATRIX viewMatrix = m_moveLookController->ViewMatrix();
	XMVECTOR clickPointNear = DirectX::XMVectorSet(mouseX, mouseY, 0.0f, 0.0f);
	XMVECTOR clickPointFar  = DirectX::XMVectorSet(mouseX, mouseY, 1.0f, 0.0f);


	// Here, we use the identity matrix for the World matrix because we don't want to translate
	// the vectors as if they were at the origin. If we did want to do that, we would use XMMatrixTranslation(eye.x, eye.y, eye.z)
	rayOrigin = XMVector3Unproject(
		clickPointNear,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		modelMatrix);		// Use the model matrix for the drawable itself because the bounding box should be offset according to that transformation

	rayDestination = XMVector3Unproject(
		clickPointFar,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		projectionMatrix,
		viewMatrix,
		modelMatrix);

	rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestination, rayOrigin));

	// Determine if the mouse ray intersects the bounding box. If so, pass it along to the root node
	//		- Have to add a check here to make sure the bounding box is not null because we don't yet have a 
	//		- a bounding box for terrain
	if(m_boundingBox != nullptr && m_boundingBox->RayIntersectionTest(rayOrigin, rayDirection, distance))
		return m_rootNode->IsMouseHovered(clickPointNear, clickPointFar, projectionMatrix, distance);

	return false;
}


#ifndef NDEBUG
void Model::DrawImGui(std::string id)
{
	ImGui::Checkbox(("Model: Draw Bounding Box##" + id).c_str(), &m_drawBoundingBox);
	m_rootNode->DrawImGui(id);
}

bool Model::NeedDrawBoundingBox()
{
	// Return true if any model/modelnode needs its boundingbox to be drawn
	return m_drawBoundingBox || m_rootNode->NeedDrawBoundingBox();
}

void Model::DrawBoundingBox(const XMMATRIX& parentModelMatrix, const XMMATRIX& projectionMatrix)
{
	// Draw the bounding box for the model if necessary then pass the call to the root node
	if (m_drawBoundingBox && m_boundingBox != nullptr)
		m_boundingBox->Draw(parentModelMatrix, m_moveLookController->ViewMatrix(), projectionMatrix);

	// NOTE: Don't need to pass the model matrix to the root node because it should already have 
	// an updated accumulated model matrix
	m_rootNode->DrawBoundingBox(projectionMatrix);
}
#endif