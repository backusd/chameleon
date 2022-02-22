#pragma once
#include "pch.h"
#include "Bindable.h"



class SamplerState : public Bindable
{
public:
	SamplerState(std::shared_ptr<DeviceResources> deviceResources);

	void Filter(D3D11_FILTER filter) { m_desc.Filter = filter; LoadChanges(); }
	void AddressU(D3D11_TEXTURE_ADDRESS_MODE mode) { m_desc.AddressU = mode; LoadChanges(); }
	void AddressV(D3D11_TEXTURE_ADDRESS_MODE mode) { m_desc.AddressV = mode; LoadChanges(); }
	void AddressW(D3D11_TEXTURE_ADDRESS_MODE mode) { m_desc.AddressW = mode; LoadChanges(); }
	void MipLODBias(float bias) { m_desc.MipLODBias = bias; LoadChanges(); }
	void MaxAnisotropy(unsigned int max) { m_desc.MaxAnisotropy = max; LoadChanges(); }
	void ComparisonFunc(D3D11_COMPARISON_FUNC compareFunc) { m_desc.ComparisonFunc = compareFunc; LoadChanges(); }
	void BorderColor0(float color) { m_desc.BorderColor[0] = color; LoadChanges(); }
	void BorderColor1(float color) { m_desc.BorderColor[1] = color; LoadChanges(); }
	void BorderColor2(float color) { m_desc.BorderColor[2] = color; LoadChanges(); }
	void BorderColor3(float color) { m_desc.BorderColor[3] = color; LoadChanges(); }
	void MinLOD(float min) { m_desc.MinLOD = min; LoadChanges(); }
	void MaxLOD(float max) { m_desc.MaxLOD = max; LoadChanges(); }


	void Bind() override;

	void ResetState();

private:
	void LoadChanges();

	D3D11_SAMPLER_DESC m_desc;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};