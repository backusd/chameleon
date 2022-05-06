#pragma once
#include "pch.h"
#include "Drawable.h"

#include <string>

class Player : public Drawable
{
public:
	Player(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string modelFilename);

	DirectX::XMFLOAT3 CenterOfModel();

	void MoveTo(DirectX::XMFLOAT3 location, float speed = 10.0f);
	void MoveForward(bool moveForward, float speed = 10.0f);

	void LookLeft(float angle);
	void LookRight(float angle);

protected:
	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain) override;

private:


	float m_movementSpeed;
	bool m_movingForward;
	double m_currentTime;
	double m_previousTime;

	bool m_movingToClickLocation;
	DirectX::XMFLOAT3 m_clickLocation;
	DirectX::XMFLOAT3 m_velocityVector;
	double m_startTime, m_endTime;

	bool m_turning;
	float m_yawRemainingToTurn;

	std::shared_ptr<Terrain> m_currentTerrain;
};