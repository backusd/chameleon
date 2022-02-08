#include "DrawPipeline.h"

using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

DrawPipeline::DrawPipeline(std::shared_ptr<DeviceResources> deviceResources, std::string meshName, std::string vertexShaderName, std::string pixelShaderName) :
	m_deviceResources(deviceResources),
	m_mesh(ObjectStore::GetMesh(meshName)),
	m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
	m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
	m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName))
{

}

DrawPipeline::DrawPipeline(std::shared_ptr<DeviceResources> deviceResources, 
							std::string meshName, 
							std::string vertexShaderName, 
							std::string pixelShaderName,
							std::vector<std::string> vertexShaderConstantBufferNames,
							std::vector<std::string> pixelShaderConstantBufferNames) :
	m_deviceResources(deviceResources),
	m_mesh(ObjectStore::GetMesh(meshName)),
	m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
	m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
	m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName))
{
	for (std::string name : vertexShaderConstantBufferNames)
		m_vertexShaderConstantBuffers.push_back(ObjectStore::GetConstantBuffer(name));

	for (std::string name : pixelShaderConstantBufferNames)
		m_pixelShaderConstantBuffers.push_back(ObjectStore::GetConstantBuffer(name));
}

void DrawPipeline::UpdatePSSubresource(int index, void* data)
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	context->UpdateSubresource(m_pixelShaderConstantBuffers[index].Get(), 0, nullptr, data, 0, 0);
}
void DrawPipeline::UpdateVSSubresource(int index, void* data)
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	context->UpdateSubresource(m_vertexShaderConstantBuffers[index].Get(), 0, nullptr, data, 0, 0);
}

void DrawPipeline::Update(std::shared_ptr<StepTimer> timer)
{
	for (std::shared_ptr<Renderable> renderable : m_renderables)
		renderable->Update(timer);
}

void DrawPipeline::Draw(DirectX::XMMATRIX viewProjection)
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	// Set shaders and input layout
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
	context->IASetInputLayout(m_inputLayout.Get());
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);

	// Set index and vertex buffers
	m_mesh->PreparePipeline();

	// Update what is in the PS/VS constant buffers
	/*
	PhongMaterialProperties* helium = new PhongMaterialProperties();
	helium->Material.Emissive = XMFLOAT4(0.4f, 0.14f, 0.14f, 1.0f);
	helium->Material.Ambient = XMFLOAT4(1.0f, 0.75f, 0.75f, 1.0f);
	helium->Material.Diffuse = XMFLOAT4(1.0f, 0.6f, 0.6f, 1.0f);
	helium->Material.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	helium->Material.SpecularPower = 6.0f;
	context->UpdateSubresource(m_pixelShaderConstantBuffers[0].Get(), 0, nullptr, helium, 0, 0);
	*/


	// Set the pixel shader and vertex shader constant buffers
	SetPSConstantBuffers();
	SetVSConstantBuffers();

	// loop over each renderable and update the vertex constant buffer and then draw
	D3D11_MAPPED_SUBRESOURCE ms;
	UINT indexCount = m_mesh->IndexCount();
	for (std::shared_ptr<Renderable> renderable : m_renderables)
	{		
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(m_vertexShaderConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);


		XMFLOAT4X4 model, modelViewProjection, inverseTransposeModel;
		XMMATRIX _model = renderable->GetModelMatrix();
		DirectX::XMStoreFloat4x4(&model, _model);
		DirectX::XMStoreFloat4x4(&modelViewProjection, _model * viewProjection);
		DirectX::XMStoreFloat4x4(&inverseTransposeModel, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, _model)));
		

		ModelViewProjectionConstantBuffer* mappedBuffer = (ModelViewProjectionConstantBuffer*)ms.pData;
		mappedBuffer->model = model;
		mappedBuffer->modelViewProjection = modelViewProjection;
		mappedBuffer->inverseTransposeModel = inverseTransposeModel;
		/*mappedBuffer->transform = DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationZ(2.0f) *
			DirectX::XMMatrixRotationX(2.0f) *
			DirectX::XMMatrixTranslation(0.0f, 0.0f, 5.0f) *
			DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 10.0f)
		);
		*/

		context->Unmap(m_vertexShaderConstantBuffers[0].Get(), 0);

		GFX_THROW_INFO_ONLY(context->DrawIndexed(indexCount, 0u, 0u));
	}
}


void DrawPipeline::SetPSConstantBuffers()
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	if (m_pixelShaderConstantBuffers.size() > 0)
	{
		switch (m_pixelShaderConstantBuffers.size())
		{
			/*
		case 1:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 1u, buffers);
			break;
			*/
			
		case 2:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
										 m_pixelShaderConstantBuffers[1].Get() };
			context->PSSetConstantBuffers(0u, 2u, buffers);
			break;
			/*
		case 3:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 3u, buffers);
			break;
		case 4:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[3]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 4u, buffers);
			break;
		case 5:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[4]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 5u, buffers);
			break;
		case 6:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[5]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 6u, buffers);
			break;
		case 7:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[5]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[6]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 7u, buffers);
			break;
		case 8:
			ID3D11Buffer * buffers[] = { m_pixelShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[5]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[6]->GetBuffer().Get(),
										 m_pixelShaderConstantBuffers[7]->GetBuffer().Get() };
			context->PSSetConstantBuffers(0u, 8u, buffers);
			break;

			*/
		}

	}
}

void DrawPipeline::SetVSConstantBuffers()
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	if (m_vertexShaderConstantBuffers.size() > 0)
	{
		switch (m_vertexShaderConstantBuffers.size())
		{
		case 1:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0].Get() };
			context->VSSetConstantBuffers(0u, 1u, buffers);
			break;
			/*
		case 2:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 2u, buffers);
			break;
		case 3:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 3u, buffers);
			break;
		case 4:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[3]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 4u, buffers);
			break;
		case 5:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[4]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 5u, buffers);
			break;
		case 6:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[5]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 6u, buffers);
			break;
		case 7:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[5]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[6]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 7u, buffers);
			break;
		case 8:
			ID3D11Buffer * buffers[] = { m_vertexShaderConstantBuffers[0]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[1]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[2]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[3]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[4]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[5]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[6]->GetBuffer().Get(),
										 m_vertexShaderConstantBuffers[7]->GetBuffer().Get() };
			context->VSSetConstantBuffers(0u, 8u, buffers);
			break;

			*/
		}

	}
}