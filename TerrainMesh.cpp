#include "TerrainMesh.h"

using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;

TerrainMesh::TerrainMesh(std::shared_ptr<DeviceResources> deviceResources) :
	Mesh(deviceResources)
{
	m_sizeOfVertex = sizeof(TerrainVertexType);
	m_indexFormat = DXGI_FORMAT_R32_UINT;

	// Tutorial1Setup();

	Tutorial2Setup("Terrain.txt");
}

void TerrainMesh::Tutorial1Setup()
{
	/*
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	INFOMAN(m_deviceResources);

	// Vertex Buffer =================================================================================
	TerrainVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	int i, j, terrainWidth, terrainHeight, index;
	XMFLOAT4 color;
	float positionX, positionZ;

	// Set the height and width of the terrain grid.
	terrainHeight = 256;
	terrainWidth = 256;

	// Set the color of the terrain grid.
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate the number of vertices in the terrain.
	m_vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new TerrainVertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Initialize the index into the vertex and index arrays.
	index = 0;

	// Load the vertex array and index array with data.
	for (j = 0; j < (terrainHeight - 1); j++)
	{
		for (i = 0; i < (terrainWidth - 1); i++)
		{
			// Line 1 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 1 - Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Upper right
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 2 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom right.
			positionX = (float)(i + 1);
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 3 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Bottom left.
			positionX = (float)i;
			positionZ = (float)j;

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			// Line 4 - Upper left.
			positionX = (float)i;
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
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

	// Index Buffer =================================================================================
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


	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	*/
}

void TerrainMesh::Tutorial2Setup(std::string setupFilename)
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Get the terrain filename, dimensions, and so forth from the setup file.
	LoadSetupFile(setupFilename);

	// Initialize the terrain height map with the data from the bitmap file.
	LoadBitmapHeightMap();

	// Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetTerrainCoordinates();

	// Calculate the normals for the terrain data.
	CalculateNormals();

	// Load in the color map for the terrain.
	LoadColorMap();

	// Now build the 3D model of the terrain.
	BuildTerrainModel();

	// We can now release the height map since it is no longer needed in memory once the 3D terrain model has been built.
	m_heightMapVector.clear();

	// Load the rendering buffers with the terrain data.
	InitializeBuffers();

	// Release the terrain model now that the rendering buffers have been loaded.
	m_terrainModelVector.clear();
}

void TerrainMesh::LoadSetupFile(std::string filename)
{
	std::ifstream fin;
	char input;

	// Open the setup file.  If it could not open the file then exit.
	fin.open(filename);
	if (fin.fail())
	{	
		std::ostringstream oss;
		oss << "Failed to open file: " << filename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Read up to the terrain file name.
	fin.get(input);
	while (input != ':')
		fin.get(input);

	// Read in the terrain file name.
	fin >> m_terrainFilename;

	// Read up to the value of terrain height.
	fin.get(input);
	while (input != ':')
		fin.get(input);

	// Read in the terrain height.
	fin >> m_terrainHeight;

	// Read up to the value of terrain width.
	fin.get(input);
	while (input != ':')
		fin.get(input);

	// Read in the terrain width.
	fin >> m_terrainWidth;

	// Read up to the value of terrain height scaling.
	fin.get(input);
	while (input != ':')
		fin.get(input);

	// Read in the terrain height scaling.
	fin >> m_heightScale;

	// Read up to the color map file name.
	fin.get(input);
	while (input != ':')
		fin.get(input);

	// Read in the color map file name.
	fin >> m_colorMapFilename;

	// Close the setup file.
	fin.close();
}

void TerrainMesh::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;
	std::ostringstream oss;


	// Start by creating the array structure to hold the height map data.
	unsigned long long totalHeights = static_cast<unsigned long long>(m_terrainWidth) * static_cast<unsigned long long>(m_terrainHeight);
	for (int iii = 0; iii < totalHeights; ++iii)
		m_heightMapVector.push_back(std::make_unique<HeightMapType>());



	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename.c_str(), "rb");
	if (error != 0)
	{
		oss << "Failed to open bitmap file: " << m_terrainFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		oss << "Failed to read bitmap file header: " << m_terrainFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		oss << "Failed to read bitmap info header: " << m_terrainFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		oss << "Bitmap info header height and width do not match the config file height and width:" << std::endl;
		oss << "    Bitmap:" << std::endl;
		oss << "        Height: " << bitmapInfoHeader.biHeight << std::endl;
		oss << "        Width:  " << bitmapInfoHeader.biWidth << std::endl;
		oss << "    Config file:" << std::endl;
		oss << "        Height: " << m_terrainHeight << std::endl;
		oss << "        Width:  " << m_terrainWidth << std::endl;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		// Must release memory before throwing the exception
		delete[] bitmapImage;
		bitmapImage = 0;

		oss << "Failed to read expected number of bytes from bitmap image" << std::endl;
		oss << "    Expected: " << imageSize << std::endl;
		oss << "    Actual:   " << count << std::endl;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		// Must release memory before throwing the exception
		delete[] bitmapImage;
		bitmapImage = 0;

		oss << "Failed to close the file: " << m_terrainFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMapVector[index]->y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;
}

void TerrainMesh::SetTerrainCoordinates()
{
	int i, j, index;

	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMapVector[index]->x = (float)i;
			m_heightMapVector[index]->z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMapVector[index]->z += (float)(m_terrainHeight - 1);

			// Scale the height.
			m_heightMapVector[index]->y /= m_heightScale;
		}
	}
}

void TerrainMesh::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	for (unsigned int iii = 0; iii < m_vertexCount; ++iii)
		m_terrainModelVector.push_back(std::make_unique<ModelType>());

	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			m_terrainModelVector[index]->x = m_heightMapVector[index1]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index1]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index1]->z;
			m_terrainModelVector[index]->tu = 0.0f;
			m_terrainModelVector[index]->tv = 0.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index1]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index1]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index1]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index1]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index1]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index1]->b;
			index++;

			// Triangle 1 - Upper right.
			m_terrainModelVector[index]->x = m_heightMapVector[index2]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index2]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index2]->z;
			m_terrainModelVector[index]->tu = 0.0f;
			m_terrainModelVector[index]->tv = 1.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index2]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index2]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index2]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index2]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index2]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index2]->b;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModelVector[index]->x = m_heightMapVector[index3]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index3]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index3]->z;
			m_terrainModelVector[index]->tu = 1.0f;
			m_terrainModelVector[index]->tv = 0.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index3]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index3]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index3]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index3]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index3]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index3]->b;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModelVector[index]->x = m_heightMapVector[index3]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index3]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index3]->z;
			m_terrainModelVector[index]->tu = 1.0f;
			m_terrainModelVector[index]->tv = 0.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index3]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index3]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index3]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index3]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index3]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index3]->b;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModelVector[index]->x = m_heightMapVector[index2]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index2]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index2]->z;
			m_terrainModelVector[index]->tu = 0.0f;
			m_terrainModelVector[index]->tv = 1.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index2]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index2]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index2]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index2]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index2]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index2]->b;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModelVector[index]->x = m_heightMapVector[index4]->x;
			m_terrainModelVector[index]->y = m_heightMapVector[index4]->y;
			m_terrainModelVector[index]->z = m_heightMapVector[index4]->z;
			m_terrainModelVector[index]->tu = 1.0f;
			m_terrainModelVector[index]->tv = 1.0f;
			m_terrainModelVector[index]->nx = m_heightMapVector[index4]->nx;
			m_terrainModelVector[index]->ny = m_heightMapVector[index4]->ny;
			m_terrainModelVector[index]->nz = m_heightMapVector[index4]->nz;
			m_terrainModelVector[index]->r = m_heightMapVector[index4]->r;
			m_terrainModelVector[index]->g = m_heightMapVector[index4]->g;
			m_terrainModelVector[index]->b = m_heightMapVector[index4]->b;
			index++;
		}
	}
}

void TerrainMesh::InitializeBuffers()
{
	INFOMAN(m_deviceResources);

	// Vertex Buffer =================================================================================
	TerrainVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	unsigned int i;
	int terrainWidth, terrainHeight;
	XMFLOAT4 color;

	// Set the height and width of the terrain grid.
	terrainHeight = 256;
	terrainWidth = 256;

	// Set the color of the terrain grid.
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate the number of vertices in the terrain.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new TerrainVertexType[m_vertexCount];

	// Create the index array.
	indices = new unsigned long[m_indexCount];

	// Load the vertex array and index array with 3D terrain model data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_terrainModelVector[i]->x, m_terrainModelVector[i]->y, m_terrainModelVector[i]->z);
		vertices[i].texture = XMFLOAT2(m_terrainModelVector[i]->tu, m_terrainModelVector[i]->tv);
		vertices[i].normal = XMFLOAT3(m_terrainModelVector[i]->nx, m_terrainModelVector[i]->ny, m_terrainModelVector[i]->nz);
		vertices[i].color = XMFLOAT3(m_terrainModelVector[i]->r, m_terrainModelVector[i]->g, m_terrainModelVector[i]->b);
		indices[i] = i;
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

	// Index Buffer =================================================================================
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


	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void TerrainMesh::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the face normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1[0] = m_heightMapVector[index1]->x;
			vertex1[1] = m_heightMapVector[index1]->y;
			vertex1[2] = m_heightMapVector[index1]->z;

			vertex2[0] = m_heightMapVector[index2]->x;
			vertex2[1] = m_heightMapVector[index2]->y;
			vertex2[2] = m_heightMapVector[index2]->z;

			vertex3[0] = m_heightMapVector[index3]->x;
			vertex3[1] = m_heightMapVector[index3]->y;
			vertex3[2] = m_heightMapVector[index3]->z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMapVector[index]->nx = (sum[0] / length);
			m_heightMapVector[index]->ny = (sum[1] / length);
			m_heightMapVector[index]->nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;
}

void TerrainMesh::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	std::ostringstream oss;


	// Open the color map file in binary.
	error = fopen_s(&filePtr, m_colorMapFilename.c_str(), "rb");
	if (error != 0)
	{
		oss << "Failed to open bitmap file: " << m_colorMapFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		oss << "Failed to read bitmap file header: " << m_colorMapFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		oss << "Failed to read bitmap info header: " << m_colorMapFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		oss << "Bitmap info header height and width do not match the config file height and width:" << std::endl;
		oss << "    Bitmap:" << std::endl;
		oss << "        Height: " << bitmapInfoHeader.biHeight << std::endl;
		oss << "        Width:  " << bitmapInfoHeader.biWidth << std::endl;
		oss << "    Config file:" << std::endl;
		oss << "        Height: " << m_terrainHeight << std::endl;
		oss << "        Width:  " << m_terrainWidth << std::endl;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Calculate the size of the bitmap image data.  Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		// Must release memory before throwing the exception
		delete[] bitmapImage;
		bitmapImage = 0;

		oss << "Failed to read expected number of bytes from bitmap image" << std::endl;
		oss << "    Expected: " << imageSize << std::endl;
		oss << "    Actual:   " << count << std::endl;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		// Must release memory before throwing the exception
		delete[] bitmapImage;
		bitmapImage = 0;

		oss << "Failed to close the file: " << m_colorMapFilename;
		throw TerrainMeshException(__LINE__, __FILE__, oss.str());
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			m_heightMapVector[index]->b = (float)bitmapImage[k] / 255.0f;
			m_heightMapVector[index]->g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMapVector[index]->r = (float)bitmapImage[k + 2] / 255.0f;

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;
}