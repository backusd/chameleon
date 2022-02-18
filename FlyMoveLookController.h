#pragma once
#include "pch.h"

#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MoveLookController.h"

#include <cmath>
#include <memory>
#include <sstream>

#include "imgui.h"

// For some reason, the XMVECTOR operator* does not work unless I include the 
// entire namespace
using namespace DirectX;

class FlyMoveLookController : public MoveLookController
{
public:
	FlyMoveLookController(HWND hWnd);

	bool IsMoving() override;

protected:
	void ResetState() override;
	void UpdatePosition() override;

	void LookLeft() override;
	void LookRight() override;
	void LookUp() override;
	void LookDown() override;
	void MoveForward() override;
	void MoveBackward() override;


	// ImGui ====================================================================
	//
	// NOTE: This should only be used if in Debug, so consider using #if defined NDEBUG
	//


protected:
	void SetupImGui() override;
};