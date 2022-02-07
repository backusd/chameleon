#include "Scene.h"

Scene::Scene(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{

}


void Scene::Draw()
{
	INFOMAN(m_deviceResources);

	ID3D11Device5* device = m_deviceResources->D3DDevice();
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	// create vertex buffer (1 2d triangle at center of screen)
	Vertex vertices[] =
	{
		{ -1.0f,-1.0f,-1.0f	 },
		{ 1.0f,-1.0f,-1.0f	 },
		{ -1.0f,1.0f,-1.0f	 },
		{ 1.0f,1.0f,-1.0f	  },
		{ -1.0f,-1.0f,1.0f	 },
		{ 1.0f,-1.0f,1.0f	  },
		{ -1.0f,1.0f,1.0f	 },
		{ 1.0f,1.0f,1.0f	 },
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(device->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// create index buffer
	const unsigned short indices[] =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	GFX_THROW_INFO(device->CreateBuffer(&ibd, &isd, &pIndexBuffer));

	// bind index buffer
	context->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);


	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationZ(2.0f) *
				DirectX::XMMatrixRotationX(2.0f) *
				DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f,1.0f,0.5f,10.0f)
			)
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	GFX_THROW_INFO(device->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// bind constant buffer to vertex shader
	context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


	// lookup table for cube face colors
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	const ConstantBuffer2 cb2 =
	{
		{
			{1.0f,0.0f,1.0f},
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
			{1.0f,1.0f,0.0f},
			{0.0f,1.0f,1.0f},
		}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	GFX_THROW_INFO(device->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

	// bind constant buffer to pixel shader
	context->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());



	// look up the pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader = ObjectStore::GetPixelShader("basic-cube-pixel-shader");

	// bind pixel shader
	context->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	// look up the vertex shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader = ObjectStore::GetVertexShader("basic-cube-vertex-shader");
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob = ObjectStore::GetVertexShaderBlob("basic-cube-vertex-shader");

	// bind vertex shader
	context->VSSetShader(pVertexShader.Get(), nullptr, 0u);


	// input (vertex) layout (2d position only)
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		ied, (UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	));

	// bind vertex layout
	context->IASetInputLayout(pInputLayout.Get());


	// Set primitive topology to triangle list (groups of 3 vertices)
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// configure viewport
	/*
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	context->RSSetViewports(1u, &vp);
	*/


	GFX_THROW_INFO_ONLY(context->DrawIndexed((UINT)std::size(indices), 0u, 0u));
}