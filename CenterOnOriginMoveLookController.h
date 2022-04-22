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

class CenterOnOriginMoveLookController : public MoveLookController
{
public:
	CenterOnOriginMoveLookController(HWND hWnd, std::shared_ptr<DeviceResources> deviceResources);
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse) override;
	bool IsMoving() override;

private:
	void ResetState() override;
	void UpdatePosition() override;

	void ZoomIn(int mouseX, int mouseY) override;
	void ZoomOut(int mouseX, int mouseY) override;
	void MouseMove() override;

	void RotateLeftRight(float theta);
	void RotateUpDown(float theta);

	void InitializeAutomatedMove(double maxMoveTime);

	void CenterOnFace();
	void RotateLeft90();
	void RotateRight90();
	void RotateUp90();
	void RotateDown90();

	/*
	void OnLButtonDoubleClick();
	void OnMouseWheel(int wheelDelta);
	*/


	// When zooming in/out, or rotating 90, etc., we simply need to set the target location and
	// Update() will move the camera for us
	DirectX::XMFLOAT3 m_eyeTarget;
	DirectX::XMFLOAT3 m_eyeInitial;
	DirectX::XMFLOAT3 m_upTarget;
	DirectX::XMFLOAT3 m_upInitial;
	double m_moveStartTime;
	bool   m_moveCompleted;
	double m_movementMaxTime;
	double m_timeAtLastMoveUpdate;
	float  m_totalRotationAngle;
	bool m_rotatingLeftRight;
	bool m_rotatingUpDown;


	bool  m_movingToNewLocation; // zooming in/out, rotating 90', centering on closest face, etc.

	// Keep track of total time to be able to compute the time delta when an arrow key is held down
	double m_elapsedTime;




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

	DirectX::XMFLOAT3 m_cameraLookAtMax;
	DirectX::XMFLOAT3 m_cameraLookAtMin;

	DirectX::XMFLOAT3 m_cameraUpDirectionMax;
	DirectX::XMFLOAT3 m_cameraUpDirectionMin;
	
};