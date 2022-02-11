#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"


#include <memory>



class DepthStencilState
{
public:
	DepthStencilState(std::shared_ptr<DeviceResources> deviceResources, bool depthEnabled);

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetState() { return m_depthStencilState; }

private:
	std::shared_ptr<DeviceResources> m_deviceResources;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};