#include "RasterizerState.h"


RasterizerState::RasterizerState(std::shared_ptr<DeviceResources> deviceResources) :
	Bindable(deviceResources)
{
	ResetState();
}

void RasterizerState::ResetState()
{
	m_desc.FillMode = D3D11_FILL_SOLID;
	m_desc.CullMode = D3D11_CULL_NONE;
	m_desc.FrontCounterClockwise = true;
	m_desc.DepthBias = 0;
	m_desc.SlopeScaledDepthBias = 0.0f;
	m_desc.DepthBiasClamp = 0.0f;
	m_desc.DepthClipEnable = true;
	m_desc.ScissorEnable = false;
	m_desc.MultisampleEnable = false;
	m_desc.AntialiasedLineEnable = false;

	LoadChanges();
}

void RasterizerState::LoadChanges()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateRasterizerState(&m_desc, m_rasterizerState.ReleaseAndGetAddressOf())
	);
}

void RasterizerState::Bind()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->RSSetState(m_rasterizerState.Get())
	);
}