#include "HUD.h"


HUD::HUD(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources),
	m_lastUpdateTime(0)
{
	// Create two text controls - one for "FPS: " and the other for the value
	m_fpsHeader = std::make_unique<Text>(m_deviceResources);
	m_fpsHeader->SetText(L"FPS: ");
	m_fpsHeader->SetTop(10.0f);
	m_fpsHeader->SetLeft(10.0f);

	m_fpsValue = std::make_unique<Text>(m_deviceResources);
	m_fpsValue->SetTop(10.0f);
	m_fpsValue->SetLeft(40.0f);
}

void HUD::Update(std::shared_ptr<StepTimer> timer)
{
	// Only update the FPS once every second
	double currentTime = timer->GetTotalSeconds();
	if (currentTime - m_lastUpdateTime > 1.0)
	{
		std::ostringstream oss;
		oss << timer->GetFramesPerSecond();
		m_fpsValue->SetText(oss.str());

		m_lastUpdateTime = currentTime;
	}
}

void HUD::Draw()
{
	m_fpsHeader->Draw();
	m_fpsValue->Draw();
}