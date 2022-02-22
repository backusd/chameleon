#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"

#include <memory>



class ConstantBuffer
{
public:
	ConstantBuffer(std::shared_ptr<DeviceResources> deviceResources);

	template <typename T>
	void CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride);

	ID3D11Buffer* GetRawBufferPointer() { return m_buffer.Get(); }

private:;
	   std::shared_ptr<DeviceResources> m_deviceResources;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

};

template <typename T>
void ConstantBuffer::CreateBuffer(D3D11_USAGE usage, unsigned int cpuAccessFlags, unsigned int miscFlags, unsigned int structuredByteStride)
{
	INFOMAN(m_deviceResources);

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(T);
	desc.Usage = usage;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = cpuAccessFlags;
	desc.MiscFlags = miscFlags;
	desc.StructureByteStride = structuredByteStride;

	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateBuffer(
			&desc,								// Use the description we just created
			nullptr,							// Don't fill it with any data
			m_buffer.ReleaseAndGetAddressOf()	// Assign result to buffer
		)
	);
}