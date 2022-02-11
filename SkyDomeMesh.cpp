#include "SkyDomeMesh.h"

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;

SkyDomeMesh::SkyDomeMesh(std::shared_ptr<DeviceResources> deviceResources) :
	Mesh(deviceResources)
{
	m_sizeOfVertex = sizeof(SkyDomeVertexType);
	m_indexFormat = DXGI_FORMAT_R32_UINT;

	// ------------------------------------------------------
	// 
	// Load in the sky dome model.
	LoadSkyDomeModel("skydome.txt");

	// Load the sky dome into a vertex and index buffer for rendering.
	InitializeBuffers();

	// Clear the ModelType buffer
	m_model.clear();

	// Set the color at the top of the sky dome.
	m_apexColor = XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f);

	// Set the color at the center of the sky dome.
	m_centerColor = XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f);

}

void SkyDomeMesh::LoadSkyDomeModel(std::string filename)
{
	std::ifstream fin;
	char input;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		std::ostringstream oss;
		oss << "Failed to open file: " << filename;
		throw SkyDomeException(__LINE__, __FILE__, oss.str());
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	// m_model = new ModelType[m_vertexCount];
	for (int iii = 0; iii < m_vertexCount; ++iii)
		m_model.push_back(std::make_unique<ModelType>());

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i]->x >> m_model[i]->y >> m_model[i]->z;
		fin >> m_model[i]->tu >> m_model[i]->tv;
		fin >> m_model[i]->nx >> m_model[i]->ny >> m_model[i]->nz;
	}

	// Close the model file.
	fin.close();
}

void SkyDomeMesh::InitializeBuffers()
{
	INFOMAN(m_deviceResources);


	SkyDomeVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new SkyDomeVertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i]->x, m_model[i]->y, m_model[i]->z);
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SkyDomeVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

