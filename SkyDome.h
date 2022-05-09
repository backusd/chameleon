#pragma once
#include "pch.h"
#include "Drawable.h"

#include <memory>


class SkyDome : public Drawable
{
public:
	SkyDome(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController);
	SkyDome(const SkyDome&) = delete;
	SkyDome& operator=(const SkyDome&) = delete;

private:
	void CreateAndAddPSBufferArray(); // This is necessary because the cube will create an immutable constant buffer for the sky color

	DirectX::XMFLOAT4 m_apexColor;
	DirectX::XMFLOAT4 m_centerColor;



	// DEBUG SPECIFIC --------------------------------------------------------
#ifndef NDEBUG
public:
	void DrawImGuiCollapsable(std::string id) override;
	void DrawImGuiDetails(std::string id) override;
#endif

};