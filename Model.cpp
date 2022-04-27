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
			LoadMesh(*scene->mMeshes[iii]);
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

void Model::LoadMesh(const aiMesh& mesh)
{
	std::vector<OBJVertex> vertices;		// vertices for the vertex buffer
	std::vector<unsigned short> indices;	// indices for the index buffer

	vertices.reserve(mesh.mNumVertices);
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vertices.push_back(
			{
				{ mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z },
				{ 0.0f, 0.0f },	// Not sure yet how to get the texture coordinates
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i])
			}
		);
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
}

void Model::Draw(const XMMATRIX& parentModelMatrix, const XMMATRIX& projectionMatrix)
{
	// Draw the root node applying no transformations yet
	m_rootNode->Draw(parentModelMatrix, projectionMatrix);
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