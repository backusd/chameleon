#pragma once
#include "pch.h"

#include "WindowBase.h"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

class WindowManager
{
public:
	static void AddWindow(std::shared_ptr<WindowBase> window) { m_windows.push_back(window); }
	static void DestroyWindows();

	static void UpdateRenderPresent();

	static std::optional<int> ProcessMessages();


private:
	// Disallow creation of a WindowManager object
	WindowManager() {}

	//std::vector<WindowBase*> m_windows;
	static std::vector<std::shared_ptr<WindowBase>> m_windows;

};