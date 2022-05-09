#include "SamplerStateArray.h"
#include "ObjectStore.h"

SamplerStateArray::SamplerStateArray(std::shared_ptr<DeviceResources> deviceResources, SamplerStateBindingLocation bindToStage) :
	Bindable(deviceResources)
{
	switch (bindToStage)
	{
	case SamplerStateBindingLocation::COMPUTE_SHADER:	BindFunc = std::bind(&SamplerStateArray::BindCS, this); break;
	case SamplerStateBindingLocation::VERTEX_SHADER:	BindFunc = std::bind(&SamplerStateArray::BindVS, this); break;
	case SamplerStateBindingLocation::HULL_SHADER:		BindFunc = std::bind(&SamplerStateArray::BindHS, this); break;
	case SamplerStateBindingLocation::DOMAIN_SHADER:	BindFunc = std::bind(&SamplerStateArray::BindDS, this); break;
	case SamplerStateBindingLocation::GEOMETRY_SHADER:	BindFunc = std::bind(&SamplerStateArray::BindGS, this); break;
	case SamplerStateBindingLocation::PIXEL_SHADER:		BindFunc = std::bind(&SamplerStateArray::BindPS, this); break;
	}
}

void SamplerStateArray::AddSamplerState(std::string lookupName)
{
	m_samplerStates.push_back(ObjectStore::GetSamplerState(lookupName));
	m_rawSamplerStatePointers.push_back(m_samplerStates.back()->GetRawPointer());
}
void SamplerStateArray::AddSamplerState(std::shared_ptr<SamplerState> samplerState)
{
	m_samplerStates.push_back(samplerState);
	m_rawSamplerStatePointers.push_back(m_samplerStates.back()->GetRawPointer());
}
void SamplerStateArray::ClearSamplerStates()
{
	m_samplerStates.clear();
	m_rawSamplerStatePointers.clear();
}

void SamplerStateArray::Bind()
{
	BindFunc();
}

void SamplerStateArray::BindCS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->CSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}

void SamplerStateArray::BindVS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->VSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}

void SamplerStateArray::BindHS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->HSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}

void SamplerStateArray::BindDS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->DSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}

void SamplerStateArray::BindGS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->GSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}

void SamplerStateArray::BindPS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetSamplers(0u, static_cast<unsigned int>(m_rawSamplerStatePointers.size()), m_rawSamplerStatePointers.data())
	);
}