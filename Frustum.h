#pragma once
#include "pch.h"


class Frustum
{
public:
	Frustum(float screenDepth, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);
	Frustum(const Frustum&) = delete;
	~Frustum();

	void UpdateFrustum(DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);
	bool CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

private:
	float m_screenDepth;
	float m_planes[6][4];
};