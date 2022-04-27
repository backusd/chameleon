#pragma once
#include "pch.h"

#include "StepTimer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "DeviceResources.h"

#include <cmath>
#include <memory>
#include <sstream>

#include "imgui.h"

class Player;
class Terrain;

class MoveLookController
{
public:
	MoveLookController(HWND hWnd, std::shared_ptr<DeviceResources> deviceResources);

	DirectX::XMMATRIX ViewMatrix();
	DirectX::XMMATRIX ProjectionMatrix() { return m_projectionMatrix; }

	virtual void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);
	void UpdateCameraLocation();
	virtual bool IsMoving();
	void WindowResized() { UpdateProjectionMatrix(); }

	void SetPosition(DirectX::XMFLOAT3 position) { m_eyeVec = DirectX::XMLoadFloat3(&position); }

	bool LButtonIsDown() { return m_LButtonDown; }
	bool ShiftIsDown() { return m_shift; }
	bool CTRLIsDown() { return m_ctrl; }

	DirectX::XMVECTOR Position() { return m_eyeVec; }

	void SetPlayer(std::shared_ptr<Player> player);
	void SetTerrain(std::shared_ptr<Terrain> terrain) { m_terrain = terrain; }

	// Allow Player to be manually released so as to not leak resources on shutdown
	// (MoveLookController has shared_ptr to player and player has shared_ptr to it)
	// Same goes for the terrain
	void ReleaseResources() { m_player = nullptr; m_terrain = nullptr; }


	virtual void ZoomIn(int mouseX, int mouseY);
	virtual void ZoomOut(int mouseX, int mouseY);

	virtual void LookLeftRight(float angle);
	virtual void LookUpDown(float angle);

	virtual void ProcessKeyboardEvents(std::shared_ptr<Keyboard> keyboard);
	virtual void SetTimeDelta(double delta) { m_timeDelta = delta; }

	void CenterCameraBehindPlayer();

protected:
	virtual void ResetState();
	void UpdateProjectionMatrix();
	
	virtual void UpdatePosition();

	virtual void LookLeft();
	virtual void LookRight();
	virtual void LookUp();
	virtual void LookDown();
	virtual void MoveForward() {};
	virtual void MoveBackward() {};
	virtual void GoToClickLocation(float x, float y);

	virtual void MouseMove();

	HWND								m_hWnd;
	std::shared_ptr<DeviceResources>	m_deviceResources;

	DirectX::XMMATRIX					m_projectionMatrix;

	std::shared_ptr<Player>				m_player;
	std::shared_ptr<Terrain>			m_terrain;

	// Use spherical coordinates to keep track of where the camera is relative to the player
	float m_r;		// distance from the camera to the looking at point
	float m_theta;	// rotation in the xz-axis 
	float m_phi;	// rotation down from the vertical y-axis



	// Input states for keyboard
	bool  m_left;
	bool  m_right;
	bool  m_up;
	bool  m_down;
	bool  m_shift;
	bool  m_ctrl;
	bool  m_alt;
	bool  m_a;
	bool  m_w;
	bool  m_s;
	bool  m_d;

	// Input states for mouse


	double m_currentTime;
	double m_previousTime;
	double m_timeDelta;

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

};