#include "ConstantBufferArray.h"
#include "ObjectStore.h"

ConstantBufferArray::ConstantBufferArray(std::shared_ptr<DeviceResources> deviceResources, ConstantBufferBindingLocation bindToStage) :
	Bindable(deviceResources)
{
	switch (bindToStage)
	{
	case ConstantBufferBindingLocation::COMPUTE_SHADER:	 BindFunc = std::bind(&ConstantBufferArray::BindCS, this); break;
	case ConstantBufferBindingLocation::VERTEX_SHADER:	 BindFunc = std::bind(&ConstantBufferArray::BindVS, this); break;
	case ConstantBufferBindingLocation::HULL_SHADER:	 BindFunc = std::bind(&ConstantBufferArray::BindHS, this); break;
	case ConstantBufferBindingLocation::DOMAIN_SHADER:	 BindFunc = std::bind(&ConstantBufferArray::BindDS, this); break;
	case ConstantBufferBindingLocation::GEOMETRY_SHADER: BindFunc = std::bind(&ConstantBufferArray::BindGS, this); break;
	case ConstantBufferBindingLocation::PIXEL_SHADER:	 BindFunc = std::bind(&ConstantBufferArray::BindPS, this); break;
	}
}

void ConstantBufferArray::AddBuffer(std::string lookupName) 
{ 
	m_buffers.push_back(ObjectStore::GetConstantBuffer(lookupName));
}

void ConstantBufferArray::Bind()
{
	m_rawBufferPointers.clear();
	for (std::shared_ptr<ConstantBuffer> buffer : m_buffers)
		m_rawBufferPointers.push_back(buffer->GetRawBufferPointer());

	BindFunc();
}

void ConstantBufferArray::BindCS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->CSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindVS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->VSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindHS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->HSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindDS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->DSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindGS()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->GSSetConstantBuffers(0u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::BindPS()
{
	// IMPORTANT: Scene lighting is always bound to slot 0, so additional buffers MUST be bound starting at slot 1
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->PSSetConstantBuffers(1u, static_cast<unsigned int>(m_rawBufferPointers.size()), m_rawBufferPointers.data())
	);
}

void ConstantBufferArray::UpdateSubresource(int index, void* data)
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_buffers[index]->GetRawBufferPointer(), 0, nullptr, data, 0, 0)
	);
}