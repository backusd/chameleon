#pragma once
#include "pch.h"

#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <cmath>
#include <memory>
#include <sstream>

// For some reason, the XMVECTOR operator* does not work unless I include the 
// entire namespace
using namespace DirectX;

class MoveLookController
{
public:
	MoveLookController();

	DirectX::XMMATRIX ViewMatrix();

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse, HWND hWnd);

	bool IsMoving();

	void SetPosition(DirectX::XMFLOAT3 position) { m_eyeVec = DirectX::XMLoadFloat3(&position); }

	/*
	void CenterOnFace();
	void RotateLeft90();
	void RotateRight90();
	void RotateUp90();
	void RotateDown90();

	void OnLButtonDown(float mouseX, float mouseY);
	void OnLButtonUp(float mouseX, float mouseY);
	void OnLButtonDoubleClick();

	void OnMouseMove(float mouseX, float mouseY);
	void OnMouseLeave();
	void OnMouseWheel(int wheelDelta);

	void OnKeyDown(unsigned char keycode);
	void OnKeyUp(unsigned char keycode);
	*/

	bool LButtonIsDown() { return m_mouseDown; }
	bool ShiftIsDown() { return m_shift; }

	DirectX::XMVECTOR Position() { return m_eyeVec; }

	bool CTRLIsDown() { return m_ctrl; }

private:
	void ResetState();
	
	void UpdatePosition();

	void LookLeft();
	void LookRight();
	void LookUp();
	void LookDown();
	void MoveForward();
	void MoveBackward();

	//void RotateLeftRight(float theta);
	//void RotateUpDown(float theta);

	// void InitializeAutomatedMove(double maxMoveTime);

	// Input states for keyboard
	bool  m_left;
	bool  m_right;
	bool  m_up;
	bool  m_down;
	bool  m_shift;
	bool  m_ctrl;
	bool  m_alt;

	double m_currentTime;
	double m_previousTime;

	// Movement speeds
	double m_turnSpeed; // radians per second
	double m_moveSpeed; // units per second

	// Input states for mouse
	bool  m_mouseDown;



	// Eye/at/up vectors
	DirectX::XMVECTOR m_eyeVec;
	DirectX::XMVECTOR m_atVec;
	DirectX::XMVECTOR m_upVec;
	/*
	// When zooming in/out, or rotating 90, etc., we simply need to set the target location and
	// Update() will move the camera for us
	DirectX::XMFLOAT3 m_eyeTarget;
	DirectX::XMFLOAT3 m_eyeInitial;
	DirectX::XMFLOAT3 m_upTarget;
	DirectX::XMFLOAT3 m_upInitial;
	double m_moveStartTime;
	double m_updatedViewMatrixHasBeenRead;
	double m_movementMaxTime;
	double m_timeAtLastMoveUpdate;
	float  m_totalRotationAngle;
	bool m_rotatingLeftRight;
	bool m_rotatingUpDown;

	// Pointer Variables
	float m_mousePositionX;
	float m_mousePositionY;
	float m_mousePositionXNew;
	float m_mousePositionYNew;

	
	bool  m_movingToNewLocation; // zooming in/out, rotating 90', centering on closest face, etc.

	// Keep track of total time to be able to compute the time delta
	double m_elapsedTime;
	*/
};