#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"

#include <memory>

class Bindable
{
public:
	Bindable(std::shared_ptr<DeviceResources> deviceResources);

	virtual void Bind() = 0;

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
};