#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStore.h"
#include "StepTimer.h"
#include "HLSLStructures.h"
#include "MoveLookController.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Terrain.h"
#include "SkyDomeMesh.h"
#include "TerrainMesh.h"
#include "Frustum.h"
#include "CenterOnOriginMoveLookController.h"

#include "Drawable.h"
#include "Box.h"
#include "SkyDome.h"
#include "Terrain.h"
#include "Suzanne.h"
#include "Lighting.h"
#include "Sphere.h"
#include "Nanosuit.h"

#include <memory>
#include <vector>



class CenterOnOriginScene
{
public:
	CenterOnOriginScene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd);
	CenterOnOriginScene(const CenterOnOriginScene&) = delete;
	CenterOnOriginScene& operator=(const CenterOnOriginScene&) = delete;

	void WindowResized();
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);
	void Draw();

private:
	void CreateWindowSizeDependentResources();
	void CreateAndBindModelViewProjectionBuffer();


	HWND												m_hWnd;
	std::shared_ptr<DeviceResources>					m_deviceResources;

	DirectX::XMMATRIX									m_projectionMatrix;

	// Light Properties
	std::shared_ptr<Lighting>							m_lighting;

	// Cameras
	std::shared_ptr<CenterOnOriginMoveLookController>	m_moveLookController;

	// Drawables
	std::vector<std::shared_ptr<Drawable>>				m_drawables;


	// ImGui ====================================================================
	//
	// NOTE: This should only be used if in Debug, so consider using #if defined NDEBUG
	//

public:
	void DrawImGui();

};
