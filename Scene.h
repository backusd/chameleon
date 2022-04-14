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
#include "FlyMoveLookController.h"
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



class Scene
{
public:
	Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd);
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	~Scene();

	void WindowResized();
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);
	void Draw();

	template <typename T>
	std::shared_ptr<T> AddDrawable();

	void SetPlayer(std::shared_ptr<Nanosuit> player) { m_moveLookController->SetPlayer(player); }

private:
	void CreateWindowSizeDependentResources();
	void CreateAndBindModelViewProjectionBuffer();


	HWND												m_hWnd;
	std::shared_ptr<DeviceResources>					m_deviceResources;

	DirectX::XMMATRIX									m_projectionMatrix;

	// Light Properties
	std::shared_ptr<Lighting>							m_lighting;

	// Cameras
	std::shared_ptr<MoveLookController>					m_moveLookController;


	// Drawables
	std::vector<std::shared_ptr<Drawable>>				m_drawables;
	std::shared_ptr<Terrain>							m_terrain;




#ifndef NDEBUG
public:
	void DrawImGui();

private:
	void UpdateMoveLookControllerSelection();
	std::shared_ptr<FlyMoveLookController>				m_flyMoveLookController;
	bool												m_useFlyMoveLookController;
#endif
};

template <typename T>
std::shared_ptr<T> Scene::AddDrawable()
{
	std::shared_ptr<T> newItem = std::make_shared<T>(m_deviceResources, m_moveLookController);
	newItem->SetProjectionMatrix(m_projectionMatrix);
	m_drawables.push_back(newItem);
	return newItem;
}