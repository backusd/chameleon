#include "Drawable.h"

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, BasicModelType modelType) :
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
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_rootNode(nullptr),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
{
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);

	switch (modelType)
	{
	case BasicModelType::Plane:
		m_rootNode->AddMesh(ObjectStore::GetMesh("plane-mesh"));
		m_name = "Plane";
		break;
	case BasicModelType::Cube:
		m_rootNode->AddMesh(ObjectStore::GetMesh("cube-mesh")); // NOT TESTED
		m_name = "Cube";
		break;
	case BasicModelType::Sphere:
		m_rootNode->AddMesh(ObjectStore::GetMesh("sphere-mesh"));  // NOT TESTED
		m_name = "Sphere";
		break;
	case BasicModelType::Cylinder:
		m_rootNode->AddMesh(ObjectStore::GetMesh("cylinder-mesh"));  // NOT TESTED
		m_name = "Cylinder";
		break;
	}
}

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh) :
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
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_rootNode(nullptr),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
{
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);
	m_rootNode->AddMesh(mesh);
}

Drawable::Drawable(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string filename) :
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
	OnMouseNotHover([]() {}),
	OnMouseClick([]() {}),
	OnRightMouseClick([]() {}),
	m_material(nullptr),
	m_name("Unnamed Drawable"),
	m_rootNode(nullptr),
	m_boundingBox(nullptr)
#ifndef NDEBUG
	, m_drawBoundingBox(false)
#endif
{
	// Getting the stem gets just the filename without the extension
	m_name = std::filesystem::path(filename).stem().string();

	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene == nullptr)
	{
		throw DrawableException(__LINE__, __FILE__, "AssImp ReadFile Error:\n" + std::string(imp.GetErrorString()));
	}

	// Build up the vector of all meshes for this model
	std::string meshLookupName;
	for (unsigned int iii = 0; iii < scene->mNumMeshes; ++iii)
	{
		// Create a unique lookup name for the mesh
		meshLookupName = filename + "-" + std::string(scene->mMeshes[iii]->mName.C_Str());

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

void Drawable::LoadMesh(const aiMesh& mesh, const aiMaterial* const* materials)
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

void Drawable::UpdateHelper(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain)
{
	// This function just calls the Update() method which may or may not be implemented by
	// a derived class.It also handles updating the model so the derived class doesn't have to
	this->Update(timer, terrain);

	// Update the model matrix for each node in the hierarchy
	m_rootNode->Update(GetModelMatrix());
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

	// Drawing the root node will recursively draw all children nodes
	m_rootNode->Draw(m_projectionMatrix);

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
		DrawBoundingBox(GetModelMatrix(), m_projectionMatrix);
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
		GetModelMatrix());		// Use the model matrix for the drawable itself because the bounding box should be offset according to that transformation

	rayDestination = XMVector3Unproject(
		clickPointFar,
		viewport.TopLeftX, viewport.TopLeftY,
		viewport.Width, viewport.Height,
		0, 1,
		m_projectionMatrix,
		viewMatrix,
		GetModelMatrix());

	rayDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(rayDestination, rayOrigin));

	// Determine if the mouse ray intersects the bounding box. If so, pass it along to the root node
	//		- Have to add a check here to make sure the bounding box is not null because we don't yet have a 
	//		- a bounding box for terrain
	// NOTE: We do NOT have to pass the model matrix because each node already keeps track of it's accumulated model matrix
	if (m_boundingBox != nullptr && m_boundingBox->RayIntersectionTest(rayOrigin, rayDirection, distance))
		return m_rootNode->IsMouseHovered(clickPointNear, clickPointFar, m_projectionMatrix, distance);

	return false;
}



#ifndef NDEBUG
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

	ImGui::Checkbox(("Draw Bounding Box##" + id).c_str(), &m_drawBoundingBox);
	m_rootNode->DrawImGui(id);
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
	return m_drawBoundingBox || m_rootNode->NeedDrawBoundingBox();
}

void Drawable::DrawBoundingBox(const XMMATRIX& parentModelMatrix, const XMMATRIX& projectionMatrix)
{
	// Draw the bounding box for the model if necessary then pass the call to the root node
	if (m_drawBoundingBox && m_boundingBox != nullptr)
		m_boundingBox->Draw(parentModelMatrix, m_moveLookController->ViewMatrix(), projectionMatrix);

	// NOTE: Don't need to pass the model matrix to the root node because it should already have 
	// an updated accumulated model matrix
	m_rootNode->DrawBoundingBox(projectionMatrix);
}
#endif