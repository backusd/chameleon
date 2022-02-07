#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStore.h"

#include <memory>

class Scene
{
public:
	class Scene(std::shared_ptr<DeviceResources> deviceResources);
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void Draw();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

};


/*
IASetIndexBuffer
IASetInputLayout
IASetPrimitiveTopology
IASetVertexBuffers

VSSetConstantBuffers
VSSetSamplers
VSSetShader
VSSetShaderResources

PSSetConstantBuffers
PSSetSamplers
PSSetShader
PSSetShaderResources



*/