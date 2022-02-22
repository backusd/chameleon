#include "InputLayout.h"

InputLayout::InputLayout(std::shared_ptr<DeviceResources> deviceResources, std::wstring vertexShaderFile) :
	Bindable(deviceResources)
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO(D3DReadFileToBlob(vertexShaderFile.c_str(), &m_blob));
}

void InputLayout::AddDescription(
	std::string semanticName,
	unsigned int semanticIndex,
	DXGI_FORMAT format,
	unsigned int inputSlot,
	unsigned int alignedByteOffset,
	D3D11_INPUT_CLASSIFICATION inputSlotClass,
	unsigned int instanceDataStepRate)
{
	// Add the sematic name to a vector so they don't go out of scope prior to calling CreateLayout
	m_semanticNames.push_back(semanticName);

	D3D11_INPUT_ELEMENT_DESC desc;
	desc.SemanticName			= ""; // semantic name will be set in CreateLayout		//m_semanticNames[m_semanticNames.size() - 1].c_str();
	desc.SemanticIndex			= semanticIndex;
	desc.Format					= format;
	desc.InputSlot				= inputSlot;
	desc.AlignedByteOffset		= alignedByteOffset;
	desc.InputSlotClass			= inputSlotClass;
	desc.InstanceDataStepRate	= instanceDataStepRate;

	m_descriptions.push_back(desc);
}

void InputLayout::CreateLayout()
{
	INFOMAN(m_deviceResources);

	// Set the semantic names here so the c_str doesn't go out of scope
	for (unsigned int iii = 0; iii < m_semanticNames.size(); ++iii)
		m_descriptions[iii].SemanticName = m_semanticNames[iii].c_str();

	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateInputLayout(
			m_descriptions.data(),
			m_descriptions.size(),
			m_blob->GetBufferPointer(),
			m_blob->GetBufferSize(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// Once the layout is created, we can get rid of the semantic names
	m_semanticNames.clear();
}

void InputLayout::Bind()
{
	INFOMAN(m_deviceResources);
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->IASetInputLayout(m_inputLayout.Get())
	);
}