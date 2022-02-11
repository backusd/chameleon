#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Mesh.h"
#include "HLSLStructures.h"

#include <memory>

class SphereMesh : public Mesh
{
public:
	SphereMesh(std::shared_ptr<DeviceResources> deviceResources);


private:
	void InitializeBuffers();

	
};