#include "DrawPipeline.h"

using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

DrawPipeline::DrawPipeline(std::shared_ptr<DeviceResources> deviceResources, std::string meshName, std::string vertexShaderName, std::string pixelShaderName, std::string rasterStateName) :
	m_deviceResources(deviceResources),
	m_mesh(ObjectStore::GetMesh(meshName)),
	m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
	m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
	m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName)),
	m_rasterState(ObjectStore::GetRasterState(rasterStateName)),
	m_samplerState(nullptr)
{
	PerRendererableUpdate = [](std::shared_ptr<Renderable>, std::shared_ptr<Mesh>, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>&, std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>>&) {};
}

DrawPipeline::DrawPipeline(std::shared_ptr<DeviceResources> deviceResources, 
							std::string meshName, 
							std::string vertexShaderName, 
							std::string pixelShaderName,
							std::string rasterStateName,
							std::vector<std::string> vertexShaderConstantBufferNames,
							std::vector<std::string> pixelShaderConstantBufferNames) :
	m_deviceResources(deviceResources),
	m_mesh(ObjectStore::GetMesh(meshName)),
	m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
	m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
	m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName)),
	m_rasterState(ObjectStore::GetRasterState(rasterStateName)),
	m_samplerState(nullptr)
{
	for (std::string name : vertexShaderConstantBufferNames)
		m_vertexShaderConstantBuffers.push_back(ObjectStore::GetConstantBuffer(name));

	for (std::string name : pixelShaderConstantBufferNames)
		m_pixelShaderConstantBuffers.push_back(ObjectStore::GetConstantBuffer(name));
}

void DrawPipeline::AddPixelShaderTexture(std::string textureLookupName)
{
	m_pixelShaderTextures.push_back(ObjectStore::GetTexture(textureLookupName));
}
void DrawPipeline::SetSamplerState(std::string sampleStateLookupName)
{
	m_samplerState = ObjectStore::GetSamplerState(sampleStateLookupName);
}

void DrawPipeline::UpdatePSSubresource(int index, void* data)
{
	m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_pixelShaderConstantBuffers[index].Get(), 0, nullptr, data, 0, 0);
}
void DrawPipeline::UpdateVSSubresource(int index, void* data)
{
	m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_vertexShaderConstantBuffers[index].Get(), 0, nullptr, data, 0, 0);
}

void DrawPipeline::Update(std::shared_ptr<StepTimer> timer)
{
	for (std::shared_ptr<Renderable> renderable : m_renderables)
		renderable->Update(timer);
}

void DrawPipeline::Draw()
{
	INFOMAN(m_deviceResources);
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	// Set shaders and input layout
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
	context->IASetInputLayout(m_inputLayout.Get());
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);

	// Set index and vertex buffers
	m_mesh->PreparePipeline();

	// Set the pixel shader and vertex shader constant buffers
	SetPSConstantBuffers();
	SetVSConstantBuffers();

	// Set the raster state
	context->RSSetState(m_rasterState.Get());

	for (unsigned int iii = 0; iii < m_pixelShaderTextures.size(); ++iii)
		context->PSSetShaderResources(iii, 1, m_pixelShaderTextures[iii]->GetTexture().GetAddressOf());
	
	if (m_samplerState != nullptr)
		context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	// loop over each renderable and update the necessary buffers for each rendereable
	UINT indexCount = m_mesh->IndexCount();
	for (std::shared_ptr<Renderable> renderable : m_renderables)
	{	
		PerRendererableUpdate(renderable, m_mesh, m_vertexShaderConstantBuffers, m_pixelShaderConstantBuffers);

		GFX_THROW_INFO_ONLY(context->DrawIndexed(indexCount, 0u, 0u));
	}
}



void DrawPipeline::SetPSConstantBuffers()
{
	if (m_pixelShaderConstantBuffers.size() > 0)
	{
		switch (m_pixelShaderConstantBuffers.size())
		{
		case 1: SetPSConstantBuffers1(); break;
		case 2: SetPSConstantBuffers2(); break;
		case 3: SetPSConstantBuffers3(); break;
		case 4: SetPSConstantBuffers4(); break;
		case 5: SetPSConstantBuffers5(); break;
		case 6: SetPSConstantBuffers6(); break;
		case 7: SetPSConstantBuffers7(); break;
		case 8: SetPSConstantBuffers8(); break;
		}
	}
}

void DrawPipeline::SetVSConstantBuffers()
{
	if (m_vertexShaderConstantBuffers.size() > 0)
	{
		switch (m_vertexShaderConstantBuffers.size())
		{
		case 1: SetVSConstantBuffers1(); break;
		case 2: SetVSConstantBuffers2(); break;
		case 3: SetVSConstantBuffers3(); break;
		case 4: SetVSConstantBuffers4(); break;
		case 5: SetVSConstantBuffers5(); break;
		case 6: SetVSConstantBuffers6(); break;
		case 7: SetVSConstantBuffers7(); break;
		case 8: SetVSConstantBuffers8(); break;
		}
	}
}

void DrawPipeline::SetPSConstantBuffers1()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 1u, buffers);
}
void DrawPipeline::SetPSConstantBuffers2()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 2u, buffers);
}
void DrawPipeline::SetPSConstantBuffers3()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 3u, buffers);
}
void DrawPipeline::SetPSConstantBuffers4()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get(),
								m_pixelShaderConstantBuffers[3].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 4u, buffers);
}
void DrawPipeline::SetPSConstantBuffers5()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get(),
								m_pixelShaderConstantBuffers[3].Get(),
								m_pixelShaderConstantBuffers[4].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 5u, buffers);
}
void DrawPipeline::SetPSConstantBuffers6()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get(),
								m_pixelShaderConstantBuffers[3].Get(),
								m_pixelShaderConstantBuffers[4].Get(),
								m_pixelShaderConstantBuffers[5].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 6u, buffers);
}
void DrawPipeline::SetPSConstantBuffers7()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get(),
								m_pixelShaderConstantBuffers[3].Get(),
								m_pixelShaderConstantBuffers[4].Get(),
								m_pixelShaderConstantBuffers[5].Get(),
								m_pixelShaderConstantBuffers[6].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 7u, buffers);
}
void DrawPipeline::SetPSConstantBuffers8()
{
	ID3D11Buffer* buffers[] = { m_pixelShaderConstantBuffers[0].Get(),
								m_pixelShaderConstantBuffers[1].Get(),
								m_pixelShaderConstantBuffers[2].Get(),
								m_pixelShaderConstantBuffers[3].Get(),
								m_pixelShaderConstantBuffers[4].Get(),
								m_pixelShaderConstantBuffers[5].Get(),
								m_pixelShaderConstantBuffers[6].Get(),
								m_pixelShaderConstantBuffers[7].Get() };
	m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(0u, 8u, buffers);
}




void DrawPipeline::SetVSConstantBuffers1()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 1u, buffers);
}
void DrawPipeline::SetVSConstantBuffers2()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 2u, buffers);
}
void DrawPipeline::SetVSConstantBuffers3()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 3u, buffers);
}
void DrawPipeline::SetVSConstantBuffers4()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get(),
								m_vertexShaderConstantBuffers[3].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 4u, buffers);
}
void DrawPipeline::SetVSConstantBuffers5()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get(),
								m_vertexShaderConstantBuffers[3].Get(),
								m_vertexShaderConstantBuffers[4].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 5u, buffers);
}
void DrawPipeline::SetVSConstantBuffers6()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get(),
								m_vertexShaderConstantBuffers[3].Get(),
								m_vertexShaderConstantBuffers[4].Get(),
								m_vertexShaderConstantBuffers[5].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 6u, buffers);
}
void DrawPipeline::SetVSConstantBuffers7()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get(),
								m_vertexShaderConstantBuffers[3].Get(),
								m_vertexShaderConstantBuffers[4].Get(),
								m_vertexShaderConstantBuffers[5].Get(),
								m_vertexShaderConstantBuffers[6].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 7u, buffers);
}
void DrawPipeline::SetVSConstantBuffers8()
{
	ID3D11Buffer* buffers[] = { m_vertexShaderConstantBuffers[0].Get(),
								m_vertexShaderConstantBuffers[1].Get(),
								m_vertexShaderConstantBuffers[2].Get(),
								m_vertexShaderConstantBuffers[3].Get(),
								m_vertexShaderConstantBuffers[4].Get(),
								m_vertexShaderConstantBuffers[5].Get(),
								m_vertexShaderConstantBuffers[6].Get(),
								m_vertexShaderConstantBuffers[7].Get() };
	m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, 8u, buffers);
}