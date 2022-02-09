#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "Text.h"
#include "StepTimer.h"

#include <sstream>
#include <memory>

class HUD
{
public:
	HUD(std::shared_ptr<DeviceResources> deviceResources);

	void Update(std::shared_ptr<StepTimer> timer);
	void Draw();

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	std::unique_ptr<Text> m_fpsHeader;
	std::unique_ptr<Text> m_fpsValue;
	double m_lastUpdateTime;
};