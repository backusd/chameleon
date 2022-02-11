#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Renderable.h"

#include <memory>


class SkyDome : public Renderable
{
public:
	SkyDome();
	SkyDome(const SkyDome&) = delete;
	SkyDome& operator=(const SkyDome&) = delete;

	// Don't update the terrain - leave it static
	void Update(std::shared_ptr<StepTimer> timer) override {}


private:




};