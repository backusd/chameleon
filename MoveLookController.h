#pragma once
#include "pch.h"

#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <cmath>
#include <memory>
#include <sstream>

#include "imgui.h"

// For some reason, the XMVECTOR operator* does not work unless I include the 
// entire namespace
using namespace DirectX;

class MoveLookController
{
public:
	MoveLookController(HWND hWnd);

	DirectX::XMMATRIX ViewMatrix();

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);

	virtual bool IsMoving();

	void SetPosition(DirectX::XMFLOAT3 position) { m_eyeVec = DirectX::XMLoadFloat3(&position); }

	bool LButtonIsDown() { return m_LButtonDown; }
	bool ShiftIsDown() { return m_shift; }
	bool CTRLIsDown() { return m_ctrl; }

	DirectX::XMVECTOR Position() { return m_eyeVec; }

	

protected:
	virtual void ResetState();
	
	virtual void UpdatePosition();

	virtual void LookLeft();
	virtual void LookRight();
	virtual void LookUp();
	virtual void LookDown();
	virtual void MoveForward();
	virtual void MoveBackward();

	virtual void ZoomIn(int mouseX, int mouseY);
	virtual void ZoomOut(int mouseX, int mouseY);
	virtual void MouseMove();

	HWND m_hWnd;

	// Input states for keyboard
	bool  m_left;
	bool  m_right;
	bool  m_up;
	bool  m_down;
	bool  m_shift;
	bool  m_ctrl;
	bool  m_alt;

	// Input states for mouse


	double m_currentTime;
	double m_previousTime;

	// Movement speeds
	double m_turnSpeed; // radians per second
	double m_moveSpeed; // units per second

	// Input states for mouse
	bool  m_LButtonDown, m_RButtonDown, m_MButtonDown;
	float m_mouseDownInitialPositionX, m_mouseDownInitialPositionY;
	float m_mouseCurrentPositionX, m_mouseCurrentPositionY;



	// Eye/at/up vectors
	DirectX::XMVECTOR m_eyeVec;
	DirectX::XMVECTOR m_atVec;
	DirectX::XMVECTOR m_upVec;
	
	// When zooming in/out, or rotating 90, etc., we simply need to set the target location and
	// Update() will move the camera for us
	/*
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

	// Keep track of total time to be able to compute the time delta
	double m_elapsedTime;
	*/



	// ImGui ====================================================================
	//
	// NOTE: This should only be used if in Debug, so consider using #if defined NDEBUG
	//

public:
	void DrawImGui();
	void LoadImGuiValues();
	void UpdateImGuiValues();

protected:
	virtual void SetupImGui();

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