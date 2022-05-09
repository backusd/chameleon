#pragma once
#include "pch.h"
#include "Bindable.h"
#include "SamplerState.h"


#include <vector>
#include <functional>

enum class SamplerStateBindingLocation
{
	COMPUTE_SHADER,
	VERTEX_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER
};

class SamplerStateArray : public Bindable
{
public:
	SamplerStateArray(std::shared_ptr<DeviceResources> deviceResources, SamplerStateBindingLocation bindToStage);

	void AddSamplerState(std::string lookupName);
	void AddSamplerState(std::shared_ptr<SamplerState> samplerState);
	void ClearSamplerStates();
	
	void Bind() override;

	ID3D11SamplerState* GetRawPointer(int index) { return m_samplerStates[index]->GetRawPointer(); }


private:
	std::function<void()> BindFunc;

	void BindCS();
	void BindVS();
	void BindHS();
	void BindDS();
	void BindGS();
	void BindPS();

	std::vector<ID3D11SamplerState*> m_rawSamplerStatePointers;
	std::vector<std::shared_ptr<SamplerState>> m_samplerStates;
};