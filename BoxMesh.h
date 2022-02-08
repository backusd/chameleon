#pragma once
#include "pch.h"
#include "Mesh.h"
#include "HLSLStructures.h"


class BoxMesh : public Mesh
{
public:
	BoxMesh(std::shared_ptr<DeviceResources> deviceResources);
};