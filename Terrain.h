#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Renderable.h"

#include <memory>




class Terrain : public Renderable
{
public:
	Terrain();
	Terrain(const Terrain&) = delete;
	Terrain& operator=(const Terrain&) = delete;

	// Don't update the terrain - leave it static
	void Update(std::shared_ptr<StepTimer> timer) override {}

private:




};