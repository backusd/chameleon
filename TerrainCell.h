#pragma once
#include "pch.h"
#include "Drawable.h"

#include <string>

#include <DirectXCollision.h>

class TerrainCell : public Drawable
{
public:
	TerrainCell(std::shared_ptr<DeviceResources> deviceResources, 
				std::shared_ptr<MoveLookController> moveLookController,
				std::string meshLookupName);

	void PreDrawUpdate() override {}
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) override {}

	bool ContainsPoint(float x, float z);
	float GetHeight(float x, float z);
	float GetMinX();
	float GetMaxX();
	float GetMinY();
	float GetMaxY();
	float GetMinZ();
	float GetMaxZ();
	bool GetClickLocation(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3& clickLocation, float& distance);


private:

};