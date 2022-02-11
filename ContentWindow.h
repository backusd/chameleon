#pragma once
#include "pch.h"


#include "WindowBase.h"
#include "ChameleonException.h"
#include "DeviceResources.h"
#include "StepTimer.h"

#include <memory>
#include <map>
#include <string>


// Global objects
#include "CharacterState.h"
#include "ObjectStore.h"

// System objects
#include "CPU.h"

// Input
#include "Keyboard.h"
#include "Mouse.h"

// Network
#include "Network.h"

// Heads Up Display (HUD)
#include "HUD.h"

// 3D Scene
#include "Scene.h"

#include "BoxMesh.h"
#include "Terrain.h"
#include "TerrainMesh.h"
#include "SkyDomeMesh.h"
#include "DepthStencilState.h"

/*
#include "CPUStatistics.h"
#include "InputClass.h"
#include "StateClass.h"
#include "BlackForestClass.h"
#include "UserInterfaceClass.h"
#include "NetworkClass.h"
*/



class ContentWindow : public WindowBase
{
public:
	ContentWindow(int width, int height, const char* name);
	~ContentWindow();

	float Height();
	float Width();

	void Update() override;
	bool Render() override;
	void Present() override;

	void Destroy() override;

protected:
	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	LRESULT OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	LRESULT OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	LRESULT OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	LRESULT OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	LRESULT OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
	LRESULT OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	void DiscardGraphicsResources();

	void ObjectStoreAddShaders();
	void ObjectStoreAddMeshes();
	void ObjectStoreAddConstantBuffers();
	void ObjectStoreAddRasterStates();
	void ObjectStoreAddSamplerStates();
	void ObjectStoreAddTextures();
	void ObjectStoreAddDepthStencilStates();

	std::shared_ptr<DeviceResources> m_deviceResources;
	std::shared_ptr<StepTimer> m_timer;

	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
	
	
	std::shared_ptr<CPU> m_cpu;

	std::shared_ptr<Keyboard> m_keyboard;
	std::shared_ptr<Mouse> m_mouse;

	std::shared_ptr<Network> m_network;

	std::shared_ptr<HUD> m_hud;
	std::shared_ptr<Scene> m_scene;


	// std::ostringstream oss;


	/*
	std::unique_ptr<CPUStatistics> m_cpuStatistics;
	std::shared_ptr<InputClass> m_inputClass;
	std::shared_ptr<UserInterfaceClass> m_userInterface;

	std::unique_ptr<StateClass> m_stateClass;
	std::shared_ptr<BlackForestClass> m_blackForest;

	std::unique_ptr<NetworkClass> m_network;
	*/












};