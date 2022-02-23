#include "DrawPipeline.h"

using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

DrawPipeline::DrawPipeline(
	std::shared_ptr<DeviceResources> deviceResources, 
	std::string meshName, std::string vertexShaderName, 
	std::string pixelShaderName, 
	std::string rasterStateName,
	std::string depthStencilStateName) :
		m_deviceResources(deviceResources),
		m_mesh(ObjectStore::GetMesh(meshName)),
		m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
		m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
		m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName)),
		m_rasterizerState(ObjectStore::GetRasterizerState(rasterStateName)),
		m_depthStencilState(ObjectStore::GetDepthStencilState(depthStencilStateName)),
		m_vertexShaderConstantBufferArray(nullptr),
		m_pixelShaderConstantBufferArray(nullptr),
		m_samplerState(nullptr)
{
	PerRendererableUpdate = [](std::shared_ptr<Renderable>, std::shared_ptr<Mesh>, std::shared_ptr<ConstantBufferArray>, std::shared_ptr<ConstantBufferArray>) {};
}

DrawPipeline::DrawPipeline(
	std::shared_ptr<DeviceResources> deviceResources,
	std::string meshName,
	std::string vertexShaderName,
	std::string pixelShaderName,
	std::string rasterStateName,
	std::string depthStencilStateName,
	std::string vertexShaderConstantBufferArrayName) :
	m_deviceResources(deviceResources),
	m_mesh(ObjectStore::GetMesh(meshName)),
	m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
	m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
	m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName)),
	m_rasterizerState(ObjectStore::GetRasterizerState(rasterStateName)),
	m_depthStencilState(ObjectStore::GetDepthStencilState(depthStencilStateName)),
	m_vertexShaderConstantBufferArray(ObjectStore::GetConstantBufferArray(vertexShaderConstantBufferArrayName)),
	m_pixelShaderConstantBufferArray(nullptr),
	m_samplerState(nullptr)
{
	PerRendererableUpdate = [](std::shared_ptr<Renderable>, std::shared_ptr<Mesh>, std::shared_ptr<ConstantBufferArray>, std::shared_ptr<ConstantBufferArray>) {};
}

DrawPipeline::DrawPipeline(
	std::shared_ptr<DeviceResources> deviceResources, 
	std::string meshName, 
	std::string vertexShaderName, 
	std::string pixelShaderName,
	std::string rasterStateName,
	std::string depthStencilStateName,
	std::string vertexShaderConstantBufferArrayName,
	std::string pixelShaderConstantBufferArrayName) :
		m_deviceResources(deviceResources),
		m_mesh(ObjectStore::GetMesh(meshName)),
		m_vertexShader(ObjectStore::GetVertexShader(vertexShaderName)),
		m_inputLayout(ObjectStore::GetInputLayout(vertexShaderName)),
		m_pixelShader(ObjectStore::GetPixelShader(pixelShaderName)),
		m_rasterizerState(ObjectStore::GetRasterizerState(rasterStateName)),
		m_depthStencilState(ObjectStore::GetDepthStencilState(depthStencilStateName)),
		m_vertexShaderConstantBufferArray(ObjectStore::GetConstantBufferArray(vertexShaderConstantBufferArrayName)),
		m_pixelShaderConstantBufferArray(ObjectStore::GetConstantBufferArray(pixelShaderConstantBufferArrayName)),
		m_samplerState(nullptr)
{
	PerRendererableUpdate = [](std::shared_ptr<Renderable>, std::shared_ptr<Mesh>, std::shared_ptr<ConstantBufferArray>, std::shared_ptr<ConstantBufferArray>) {};
}

void DrawPipeline::AddPixelShaderTexture(std::string textureLookupName)
{
	m_pixelShaderTextures.push_back(ObjectStore::GetTexture(textureLookupName));
}
void DrawPipeline::SetSamplerState(std::string sampleStateLookupName)
{
	m_samplerState = ObjectStore::GetSamplerState(sampleStateLookupName);
}

void DrawPipeline::UpdateVSSubresource(int index, void* data)
{
	m_vertexShaderConstantBufferArray->UpdateSubresource(index, data);
}
void DrawPipeline::UpdatePSSubresource(int index, void* data)
{
	m_pixelShaderConstantBufferArray->UpdateSubresource(index, data);
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
	m_inputLayout->Bind();
	m_vertexShader->Bind();
	m_pixelShader->Bind();

	// Set index and vertex buffers
	m_mesh->Bind();

	// Set the pixel shader and vertex shader constant buffers
	if (m_vertexShaderConstantBufferArray != nullptr) m_vertexShaderConstantBufferArray->Bind();
	if (m_pixelShaderConstantBufferArray != nullptr) m_pixelShaderConstantBufferArray->Bind();

	// Set the raster state
	m_rasterizerState->Bind();

	for (unsigned int iii = 0; iii < m_pixelShaderTextures.size(); ++iii)
		context->PSSetShaderResources(iii, 1, m_pixelShaderTextures[iii]->GetTexture().GetAddressOf());
	
	if (m_samplerState != nullptr) m_samplerState->Bind();

	// Set the depth stencil state
	context->OMSetDepthStencilState(m_depthStencilState->GetState().Get(), 1);

	// loop over each renderable and update the necessary buffers for each rendereable
	UINT indexCount = m_mesh->IndexCount();
	for (std::shared_ptr<Renderable> renderable : m_renderables)
	{	
		//PerRendererableUpdate(renderable, m_mesh, m_vertexShaderConstantBuffers, m_pixelShaderConstantBuffers);
		PerRendererableUpdate(renderable, m_mesh, m_vertexShaderConstantBufferArray, m_pixelShaderConstantBufferArray);

		GFX_THROW_INFO_ONLY(context->DrawIndexed(indexCount, 0u, 0u));
	}
}