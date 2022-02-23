#include "DepthStencilState.h"


DepthStencilState::DepthStencilState(std::shared_ptr<DeviceResources> deviceResources, unsigned int stencilReferenceNumber) :
	Bindable(deviceResources),
	m_stencilReferenceNumber(stencilReferenceNumber)
{
	ResetState();
	LoadChanges();
}

void DepthStencilState::ResetState()
{
	m_desc.DepthEnable = true;
	m_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_desc.DepthFunc = D3D11_COMPARISON_LESS;

	m_desc.StencilEnable = true;
	m_desc.StencilReadMask = 0xFF;
	m_desc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	m_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	m_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	m_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	m_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void DepthStencilState::LoadChanges()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateDepthStencilState(&m_desc, m_depthStencilState.ReleaseAndGetAddressOf())
	);
}

void DepthStencilState::Bind()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->OMSetDepthStencilState(m_depthStencilState.Get(), m_stencilReferenceNumber)
	);
}