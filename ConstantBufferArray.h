#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ConstantBuffer.h"


#include <vector>
#include <functional>

enum class ConstantBufferBindingLocation
{
	COMPUTE_SHADER,
	VERTEX_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER
};

class ConstantBufferArray : public Bindable
{
public:
	ConstantBufferArray(std::shared_ptr<DeviceResources> deviceResources, ConstantBufferBindingLocation bindToStage);

	void AddBuffer(std::string lookupName);
	void AddBuffer(std::shared_ptr<ConstantBuffer> buffer) { m_buffers.push_back(buffer); }
	void Bind() override;

	void UpdateSubresource(int index, void* data);

	ID3D11Buffer* GetRawBufferPointer(int index) { return m_buffers[index]->GetRawBufferPointer(); }


private:
	std::function<void()> BindFunc;

	void BindCS();
	void BindVS();
	void BindHS();
	void BindDS();
	void BindGS();
	void BindPS();

	std::vector<ID3D11Buffer*> m_rawBufferPointers;
	std::vector<std::shared_ptr<ConstantBuffer>> m_buffers;
};