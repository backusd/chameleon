#pragma once
#include "pch.h"
#include "Bindable.h"
#include "Texture.h"

#include <vector>
#include <functional>

enum class TextureBindingLocation
{
	COMPUTE_SHADER = 0,
	VERTEX_SHADER = 1,
	HULL_SHADER = 2,
	DOMAIN_SHADER = 3,
	GEOMETRY_SHADER = 4,
	PIXEL_SHADER = 5
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