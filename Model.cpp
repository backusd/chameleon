#include "Model.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController)
{
	// Create the root node - will initially have no data and no name
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);
	m_rootNode->AddMesh(mesh);
}

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string fileName) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_rootNode(nullptr)
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

void Model::Draw(XMMATRIX parentModelMatrix, XMMATRIX projectionMatrix)
{
	// Draw the root node applying no transformations yet
	m_rootNode->Draw(parentModelMatrix, projectionMatrix);
}


#ifndef NDEBUG
void Model::DrawImGui(std::string id)
{
	// There is no specific data to present from the model class itself, it is just a container
	// to hold the root node
	m_rootNode->DrawImGui(id);
}
#endif