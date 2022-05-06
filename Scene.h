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
#include "BoundingBox.h"

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
#include <string>



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
	std::shared_ptr<Drawable> CreateDrawable(BasicModelType modelType);
	std::shared_ptr<Drawable> CreateDrawable(std::string modelFilename);
	std::shared_ptr<Player> CreatePlayer(std::string modelFilename);

	std::shared_ptr<MoveLookController> GetMoveLookController() { return m_moveLookController; }


	// In the future, we will need to be able to create a scene in memory before rendering it,
	// which will require that certain events happen to finally render the scene. Right now, it is
	// just used for switch the lighting PS constant buffer between the main scene and the center
	// on origin scene
	void Activate();

private:
	void CreateAndBindModelViewProjectionBuffer();
	void ProcessMouseEvents(std::shared_ptr<StepTimer> timer, std::shared_ptr<Mouse> mouse);
	void ProcessKeyboardEvents(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard);

	HWND												m_hWnd;
	std::shared_ptr<DeviceResources>					m_deviceResources;

	// Light Properties
	std::shared_ptr<Lighting>							m_lighting;

	// Cameras
	std::shared_ptr<MoveLookController>					m_moveLookController;


	// Drawables
	std::vector<std::shared_ptr<Drawable>>				m_drawables;
	std::shared_ptr<Terrain>							m_terrain;

	// Mouse Input state variables
	bool m_LButtonDown, m_RButtonDown, m_MButtonDown;
	std::shared_ptr<Drawable> m_mouseHoveredDrawable;
	float m_distanceToHoveredDrawable;
	float m_mouseClickX, m_mouseClickY;
	float m_previousMouseMoveX, m_previousMouseMoveY;

	// Keyboard Input state variables

	// Update variables
	double m_currentTime;
	double m_previousTime;


	// ----------------------------------
	std::unique_ptr<ModelNode> m_rootNode;

	// When loading a scene/model via assimp, the meshes are just stored in a flat
	// array. The hierarchy of nodes then just have an index into that array. So, 
	// for our purpose, the model we are building needs to first create a vector
	// of shared pointers to these meshes and then as we build the node hierarchy,
	// we assign out the meshes to the corresponding nodes
	std::vector<std::shared_ptr<Mesh>> m_meshes;

	// BoundingBox to excapsulate the entire Model
	std::unique_ptr<::BoundingBox>	m_boundingBox;


	// ----------------------------------



#ifndef NDEBUG
public:
	void DrawImGui();
	void ImGuiObjectClicked(std::shared_ptr<Drawable> drawable) { m_clickedObject = drawable; }

private:
	void UpdateMoveLookControllerSelection();
	std::shared_ptr<FlyMoveLookController>				m_flyMoveLookController;
	bool												m_useFlyMoveLookController;
	std::shared_ptr<Drawable>							m_clickedObject;
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