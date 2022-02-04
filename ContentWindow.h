#pragma once
#include "pch.h"


#include "WindowBase.h"
#include "ChameleonException.h"
#include "DeviceResources.h"
#include "StepTimer.h"

#include <memory>
#include <map>
#include <string>


#include "CPUStatistics.h"
#include "InputClass.h"
#include "StateClass.h"
#include "BlackForestClass.h"


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
/*
#include "inputclass.h"
#include "d3dclass.h"
#include "networkclass.h"
#include "timerclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "userinterfaceclass.h"
#include "stateclass.h"
#include "blackforestclass.h"
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

private:
	void DiscardGraphicsResources();

	Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
	
	std::shared_ptr<DeviceResources> m_deviceResources;
	StepTimer m_timer;



	std::unique_ptr<CPUStatistics> m_cpuStatistics;
	std::unique_ptr<InputClass> m_inputClass;

	std::unique_ptr<StateClass> m_stateClass;
	std::unique_ptr<BlackForestClass> m_blackForest;

	/*
	InputClass* m_Input;
	D3DClass* m_D3D;
	NetworkClass* m_Network;

	TimerClass* m_Timer;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	UserInterfaceClass* m_UserInterface;

	StateClass* m_State;
	BlackForestClass* m_BlackForest;
	*/
};