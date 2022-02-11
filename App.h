#pragma once
#include "pch.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <tchar.h>

#include "WindowManager.h"
#include "ContentWindow.h"
#include "ChameleonException.h"
#include "DeviceResources.h"


#include <memory>


class App
{
public:
	App();

	int Run();
};