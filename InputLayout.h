#pragma once
#include "pch.h"
#include "Bindable.h"

#include <vector>
#include <string>

#include <wchar.h>

class InputLayout : public Bindable
{
public:
	InputLayout(std::shared_ptr<DeviceResources> deviceResources, std::wstring vertexShaderFile);

	void AddDescription(std::string semanticName,
		unsigned int semanticIndex,
		DXGI_FORMAT format,
		unsigned int inputSlot,
		unsigned int alignedByteOffset,
		D3D11_INPUT_CLASSIFICATION inputSlotClass,
		unsigned int instanceDataStepRate);

	void CreateLayout();

	void Bind() override;

	Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderFileBlob() { return m_blob; }

private:
	std::vector<std::string> m_semanticNames;
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_descriptions;

	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};