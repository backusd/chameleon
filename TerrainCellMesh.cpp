#include "TerrainCellMesh.h"

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

TerrainCellMesh::TerrainCellMesh(std::shared_ptr<DeviceResources> deviceResources) :
	Mesh(deviceResources)
{
	m_sizeOfVertex = sizeof(TerrainVertexType);
	m_indexFormat = DXGI_FORMAT_R32_UINT;
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


}

void TerrainCellMesh::Initialize(TerrainModelType* terrainModel, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth)
{
	// Load the rendering buffers with the terrain data for this cell index.
	InitializeBuffers(nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainModel);

	// Calculuate the dimensions of this cell.
	CalculateCellDimensions();
}

void TerrainCellMesh::InitializeBuffers(int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth, TerrainModelType* terrainModel)
{
	INFOMAN(m_deviceResources);


	TerrainVertexType* vertices;
	unsigned long* indices;
	int i, j, modelIndex, index;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Calculate the number of vertices in this terrain cell.
	m_vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new TerrainVertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Setup the indexes into the terrain model data and the local vertex/index array.
	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;

	// Load the vertex array and index array with data.
	for (j = 0; j < (cellHeight - 1); j++)
	{
		for (i = 0; i < ((cellWidth - 1) * 6); i++)
		{
			vertices[index].position = XMFLOAT3(terrainModel[modelIndex].x, terrainModel[modelIndex].y, terrainModel[modelIndex].z);
			vertices[index].texture = XMFLOAT2(terrainModel[modelIndex].tu, terrainModel[modelIndex].tv);
			vertices[index].normal = XMFLOAT3(terrainModel[modelIndex].nx, terrainModel[modelIndex].ny, terrainModel[modelIndex].nz);
			vertices[index].tangent = XMFLOAT3(terrainModel[modelIndex].tx, terrainModel[modelIndex].ty, terrainModel[modelIndex].tz);
			vertices[index].binormal = XMFLOAT3(terrainModel[modelIndex].bx, terrainModel[modelIndex].by, terrainModel[modelIndex].bz);
			vertices[index].color = XMFLOAT3(terrainModel[modelIndex].r, terrainModel[modelIndex].g, terrainModel[modelIndex].b);
			indices[index] = index;
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(TerrainVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

	// Set up the description of the static index buffer.
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


	// Create a public vertex array that will be used for accessing vertex information about this cell.
	m_vertexList = new VectorType[m_vertexCount];

	// Keep a local copy of the vertex position data for this cell.
	for (i = 0; i < m_vertexCount; i++)
	{
		m_vertexList[i].x = vertices[i].position.x;
		m_vertexList[i].y = vertices[i].position.y;
		m_vertexList[i].z = vertices[i].position.z;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void TerrainCellMesh::CalculateCellDimensions()
{
	int i;
	float width, height, depth;


	// Initialize the dimensions of the node.
	m_maxWidth = -1000000.0f;
	m_maxHeight = -1000000.0f;
	m_maxDepth = -1000000.0f;

	m_minWidth = 1000000.0f;
	m_minHeight = 1000000.0f;
	m_minDepth = 1000000.0f;

	for (i = 0; i < m_vertexCount; i++)
	{
		width = m_vertexList[i].x;
		height = m_vertexList[i].y;
		depth = m_vertexList[i].z;

		// Check if the width exceeds the minimum or maximum.
		if (width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if (width < m_minWidth)
		{
			m_minWidth = width;
		}

		// Check if the height exceeds the minimum or maximum.
		if (height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if (height < m_minHeight)
		{
			m_minHeight = height;
		}

		// Check if the depth exceeds the minimum or maximum.
		if (depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if (depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// Calculate the center position of this cell.
	m_positionX = ((m_maxWidth - m_minWidth) / 2.0f) + m_minWidth;
	m_positionY = ((m_maxHeight - m_minHeight) / 2.0f) + m_minHeight;
	m_positionZ = ((m_maxDepth - m_minDepth) / 2.0f) + m_minDepth;

}

XMFLOAT3 TerrainCellMesh::GetCenter()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

float TerrainCellMesh::GetXLength()
{
	return m_maxWidth - m_minWidth;
}

float TerrainCellMesh::GetYLength()
{
	return m_maxHeight - m_minHeight;
}

float TerrainCellMesh::GetZLength()
{
	return m_maxDepth - m_minDepth;
}

bool TerrainCellMesh::ContainsPoint(float x, float z)
{
	return x <= m_maxWidth && x >= m_minWidth && z <= m_maxDepth && z >= m_minDepth;
}

float TerrainCellMesh::GetHeight(float x, float z)
{
	// Just return the height of the vertex that is closest - THIS SHOULD DEFINITELY
	// BE IMPROVED TO BE CONSTANT TIME LOOKUP
	int closestVertexIndex = 0;
	float closestVertexDistance = FLT_MAX;
	float distance;
	for (int iii = 0; iii < m_vertexCount; iii++)
	{
		distance = pow(x - m_vertexList[iii].x, 2.0f) + pow(z - m_vertexList[iii].z, 2.0f);
		if (distance < closestVertexDistance)
		{
			closestVertexDistance = distance;
			closestVertexIndex = iii;
		}
	}

	return m_vertexList[closestVertexIndex].y;
}