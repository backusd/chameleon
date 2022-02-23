#pragma once
#include "pch.h"
#include "Bindable.h"


#include <memory>



class DepthStencilState : public Bindable
{
public:
	DepthStencilState(std::shared_ptr<DeviceResources> deviceResources, unsigned int stencilReferenceNumber);

	void DepthEnable(bool enable) { m_desc.DepthEnable = enable; LoadChanges(); }
	void DepthWriteMask(D3D11_DEPTH_WRITE_MASK mask) { m_desc.DepthWriteMask = mask; LoadChanges(); }
	void DepthFunc(D3D11_COMPARISON_FUNC func) { m_desc.DepthFunc = func; LoadChanges(); }

	void StencilEnable(bool enable) { m_desc.StencilEnable = enable; LoadChanges(); }
	void StencilReadMask(UINT8 mask) { m_desc.StencilReadMask = mask; LoadChanges(); }
	void StencilWriteMask(UINT8 mask) { m_desc.StencilWriteMask = mask; LoadChanges(); }

	// Stencil operations if pixel is front-facing.
	void FrontFaceStencilFailOp(D3D11_STENCIL_OP op) { m_desc.FrontFace.StencilFailOp = op;  LoadChanges(); }
	void FrontFaceStencilDepthFailOp(D3D11_STENCIL_OP op) { m_desc.FrontFace.StencilDepthFailOp = op; LoadChanges(); }
	void FrontFaceStencilPassOp(D3D11_STENCIL_OP op) { m_desc.FrontFace.StencilPassOp = op; LoadChanges(); }
	void FrontFaceStencilFunc(D3D11_COMPARISON_FUNC func) { m_desc.FrontFace.StencilFunc = func; LoadChanges(); }

	// Stencil operations if pixel is back-facing.
	void BackFaceStencilFailOp(D3D11_STENCIL_OP op) { m_desc.BackFace.StencilFailOp = op; LoadChanges(); }
	void BackFaceStencilDepthFailOp(D3D11_STENCIL_OP op) { m_desc.BackFace.StencilDepthFailOp = op; LoadChanges(); }
	void BackFaceStencilPassOp(D3D11_STENCIL_OP op) { m_desc.BackFace.StencilPassOp = op; LoadChanges(); }
	void BackFaceStencilFunc(D3D11_COMPARISON_FUNC func) { m_desc.BackFace.StencilFunc = func; LoadChanges(); }

	void Bind() override;

	void ResetState();

private:
	void LoadChanges();

	unsigned int m_stencilReferenceNumber;

	D3D11_DEPTH_STENCIL_DESC m_desc;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};