////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT2;

BitmapClass::BitmapClass(std::shared_ptr<DeviceResources> deviceResources, int screenWidth, int screenHeight, CHAR* textureFilename, int bitmapWidth, int bitmapHeight, int locationX, int locationY) :
	m_deviceResources(deviceResources),
	m_locX(locationX),
	m_locY(locationY),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
	m_bitmapWidth(bitmapWidth),
	m_bitmapHeight(bitmapHeight),
	m_previousPosX(-1),
	m_previousPosY(-1)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;

	InitializeBuffers();
	LoadTexture(textureFilename);
}

BitmapClass::~BitmapClass()
{
	this->Shutdown();
}


void BitmapClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}


bool BitmapClass::Render(int positionX, int positionY)
{
	bool result;


	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(positionX, positionY);
	if (!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();

	return true;
}


int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}


int BitmapClass::GetLocationX()
{
	return m_locX;
}


int BitmapClass::GetLocationY()
{
	return m_locY;
}


bool BitmapClass::InitializeBuffers()
{
	VertexType* vertices;
	unsigned long* indices;
	//D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	//D3D10_SUBRESOURCE_DATA vertexData, indexData;
	int i;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;


	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	m_deviceResources->D3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	m_deviceResources->D3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void BitmapClass::ShutdownBuffers()
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


bool BitmapClass::UpdateBuffers(int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	void* verticesPtr;
	HRESULT result;


	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);




	// Update the vertex buffer: See https://stackoverflow.com/questions/26453773/direct3d-c-api-how-to-update-vertex-buffer

	// Lock the vertex buffer.
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceResources->D3DDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	m_deviceResources->D3DDeviceContext()->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


void BitmapClass::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	ID3D11Buffer* const vertexBuffers[] = { m_vertexBuffer };

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool BitmapClass::LoadTexture(CHAR* filename)
{
	m_Texture = new TextureClass(m_deviceResources, filename);

	return true;
}


void BitmapClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}