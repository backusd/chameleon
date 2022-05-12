#pragma once
#include "pch.h"
#include "Bindable.h"
#include "ConstantBuffer.h"


#include <vector>
#include <functional>

enum class ConstantBufferBindingLocation
{
	COMPUTE_SHADER = 0,
	VERTEX_SHADER = 1,
	HULL_SHADER = 2,
	DOMAIN_SHADER = 3,
	GEOMETRY_SHADER = 4,
	PIXEL_SHADER = 5
};

class ConstantBufferArray : public Bindable
{
public:
	ConstantBufferArray(std::shared_ptr<DeviceResources> deviceResources, ConstantBufferBindingLocation bindToStage);

	void AddBuffer(std::string lookupName);
	void AddBuffer(std::shared_ptr<ConstantBuffer> buffer);
	void ClearBuffers();
	
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