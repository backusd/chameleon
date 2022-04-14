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

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse) override;
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

public:
	void DrawImGui();
	void LoadImGuiValues();
	void UpdateImGuiValues();

protected:
	void SetupImGui();

	// Camera
	DirectX::XMFLOAT3 m_cameraPosition;
	DirectX::XMFLOAT3 m_cameraLookAt;
	DirectX::XMFLOAT3 m_cameraUpDirection;

	DirectX::XMFLOAT3 m_cameraPositionMax;
	DirectX::XMFLOAT3 m_cameraPositionMin;

	DirectX::XMFLOAT3 m_cameraLookAtMax;
	DirectX::XMFLOAT3 m_cameraLookAtMin;

	DirectX::XMFLOAT3 m_cameraUpDirectionMax;
	DirectX::XMFLOAT3 m_cameraUpDirectionMin;
};