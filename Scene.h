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
#include "Lighting.h"
#include "Sphere.h"
#include "Player.h"

#include <memory>
#include <vector>
#include <type_traits>



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
	std::shared_ptr<Drawable> CreateDrawable();
	std::shared_ptr<Player> CreatePlayer();

	// In the future, we will need to be able to create a scene in memory before rendering it,
	// which will require that certain events happen to finally render the scene. Right now, it is
	// just used for switch the lighting PS constant buffer between the main scene and the center
	// on origin scene
	void Activate();

private:
	void CreateAndBindModelViewProjectionBuffer();


	HWND												m_hWnd;
	std::shared_ptr<DeviceResources>					m_deviceResources;

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
	newItem->SetProjectionMatrix(m_moveLookController->ProjectionMatrix());
	m_drawables.push_back(newItem);

	// If we are creating the lighting, keep track of it
	if constexpr (std::is_same_v<Lighting, T>)
	{
		m_lighting = newItem;
	}

	return newItem;
}