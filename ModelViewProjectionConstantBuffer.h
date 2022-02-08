#pragma once
#include "pch.h"
#include "ConstantBuffer.h"
#include "HLSLStructures.h"

class ModelViewProjectionConstantBuffer : public ConstantBuffer
{
public:
	ModelViewProjectionConstantBuffer(std::shared_ptr<DeviceResources> deviceResources);


};