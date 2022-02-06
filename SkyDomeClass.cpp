////////////////////////////////////////////////////////////////////////////////
// Filename: skydomeclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SkyDomeClass.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

SkyDomeClass::SkyDomeClass(std::shared_ptr<DeviceResources> deviceResources) : 
	m_deviceResources(deviceResources),
	m_model(nullptr),
	m_vertexCount(0),
	m_indexCount(0),
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_apexColor(XMFLOAT4(0.0f, 0.245f, 0.767f, 1.0f)),
	m_centerColor(XMFLOAT4(0.02f, 0.465f, 0.986f, 1.0f))
{
	char file[] = "skydome.txt";
	LoadSkyDomeModel(file);

	InitializeBuffers();
}


SkyDomeClass::~SkyDomeClass()
{
	this->Shutdown();
}


void SkyDomeClass::Shutdown()
{
	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	// Release the sky dome model.
	ReleaseSkyDomeModel();

	return;
}


void SkyDomeClass::Render()
{
	// Render the sky dome.
	RenderBuffers();
}


int SkyDomeClass::GetIndexCount()
{
	return m_indexCount;
}


DirectX::XMFLOAT4 SkyDomeClass::GetApexColor()
{
	return m_apexColor;
}


DirectX::XMFLOAT4 SkyDomeClass::GetCenterColor()
{
	return m_centerColor;
}


void SkyDomeClass::LoadSkyDomeModel(char* filename)
{
	std::ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		throw new ChameleonException(__LINE__, __FILE__);
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
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();
}


void SkyDomeClass::ReleaseSkyDomeModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}


void SkyDomeClass::InitializeBuffers()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	ThrowIfFailed(
		m_deviceResources->D3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)
	);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	ThrowIfFailed(
		m_deviceResources->D3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)
	);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}


void SkyDomeClass::ReleaseBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void SkyDomeClass::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}