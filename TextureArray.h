#pragma once
#include "pch.h"
#include "Bindable.h"
#include "Texture.h"

#include <vector>
#include <functional>

enum class TextureBindingLocation
{
	COMPUTE_SHADER,
	VERTEX_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER
};

class TextureArray : public Bindable
{
public:
	TextureArray(std::shared_ptr<DeviceResources> deviceResources, TextureBindingLocation bindToStage);

	void AddTexture(std::string lookupName);
	void AddTexture(std::shared_ptr<Texture> texture) { m_textures.push_back(texture); }

	void Bind() override;

private:
	std::function<void()> BindFunc;

	void BindCS();
	void BindVS();
	void BindHS();
	void BindDS();
	void BindGS();
	void BindPS();

	std::vector<ID3D11ShaderResourceView*> m_rawTextureViewPointers;
	std::vector<std::shared_ptr<Texture>> m_textures;
};