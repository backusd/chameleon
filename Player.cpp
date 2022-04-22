#include "Player.h"
#include "Terrain.h"

using DirectX::XMFLOAT3;

Player::Player(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController) :
	Drawable(deviceResources, moveLookController),
	m_movementSpeed(10.0f),
	m_movingForward(false),
	m_currentTime(0.0),
	m_previousTime(0.0),
	m_currentTerrain(nullptr),
	m_movingToClickLocation(false),
	m_clickLocation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_velocityVector(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_startTime(0.0),
	m_endTime(0.0),
	m_turning(false),
	m_yawRemainingToTurn(0.0f)
{

}

void Player::Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Terrain> terrain)
{
	// If the terrain has changed, update it first
	if (m_currentTerrain != terrain)
		m_currentTerrain = terrain;

	m_currentTime = timer->GetTotalSeconds();
	double timeDelta = m_currentTime - m_previousTime;

	if (m_movingForward)
	{
		float deltaX = static_cast<float>(m_movementSpeed * sin(m_yaw) * timeDelta);
		m_position.x += deltaX;

		float deltaZ = static_cast<float>(m_movementSpeed * cos(m_yaw) * timeDelta);
		m_position.z += deltaZ;


		// stop automated move

	}
	else if (m_movingToClickLocation)
	{
		if (m_currentTime > m_endTime)
		{
			m_position = m_clickLocation;
			m_movingToClickLocation = false;
		}
		else
		{
			m_position.x += static_cast<float>(m_velocityVector.x * timeDelta);
			m_position.y += static_cast<float>(m_velocityVector.y * timeDelta);
			m_position.z += static_cast<float>(m_velocityVector.z * timeDelta);
		}
	}

	if (m_turning)
	{
		float angle = static_cast<float>(timeDelta * DirectX::XM_2PI); // Turn at PI radians per second
		if (abs(angle) > abs(m_yawRemainingToTurn))
		{
			// In this case, just turn the remaining and be done
			angle = abs(m_yawRemainingToTurn);
			m_turning = false;
		}

		// If the yaw to turn is positive, look left, otherwise look right
		if (m_yawRemainingToTurn > 0.0f)
		{
			LookLeft(angle);
			m_yawRemainingToTurn -= angle;
		}
		else
		{
			LookRight(angle);
			m_yawRemainingToTurn += angle;
		}
	}

	// Update the y position according to the terrain height
	// NOTE: For game logic, it might make sense to only update the height if
	// the player is actuall moving forward. However, getting the terrain height
	// SHOULD be constant time lookup and it helps with debugging to just make
	// sure it is always set.
	m_position.y = m_currentTerrain->GetHeight(m_position.x, m_position.z);

	m_previousTime = m_currentTime;
}

XMFLOAT3 Player::CenterOfModel()
{
	// !!!
	// NOTE: This should be updated to get the center of the model from the model itself
	// !!!

	// Top of the nanosuit is about 15.4 units, so halfway up is about 7.7
	// Scale this down by some scale factor
	return XMFLOAT3(m_position.x, m_position.y + (7.7f * m_scaleY), m_position.z);
}

void Player::MoveTo(DirectX::XMFLOAT3 location, float speed)
{
	// The actual movement will take place in the Update function, so we must
	// set certain parameters here for the movement to be triggered
	m_movingToClickLocation = true;	// Let the Update function know we are actively moving
	m_clickLocation = location;		// Set the destination location
	m_startTime = m_currentTime;	// Set the time to the time that was computed in the last Update

	// Compute the direction between start and finish
	XMFLOAT3 direction;
	direction.x = m_clickLocation.x - m_position.x;
	direction.y = m_clickLocation.y - m_position.y;
	direction.z = m_clickLocation.z - m_position.z;

	// Compute the expected end time for the movement
	XMFLOAT3 length;
	DirectX::XMStoreFloat3(&length, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&direction)));
	m_endTime = m_startTime + (length.x / speed);

	// Normalize the direction and scale by the speed to compute the velocity vector
	DirectX::XMStoreFloat3(&m_velocityVector,
		DirectX::XMVectorScale(
			DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)),
			speed
		)
	);

	// Determine the angle to turn to face the direction
	m_turning = true;
	float finalYaw = DirectX::XMVectorGetX(
		DirectX::XMVector3AngleBetweenVectors(
			DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			DirectX::XMVectorSet(direction.x, 0.0f, direction.z, 0.0f)
		)
	);

	if (direction.x < 0.0f)
		finalYaw *= -1.0f;

	m_yawRemainingToTurn = finalYaw - m_yaw;

	// If the amount is greater than PI, then the amount needs to be reduced
	if (abs(m_yawRemainingToTurn) > DirectX::XM_PI)
	{
		// If the yaw to turn is positive, it will need to become negative
		float factor = (m_yawRemainingToTurn > 0.0f) ? -1.0f : 1.0f;

		// New angle is compute be subtracting from 2PI
		m_yawRemainingToTurn = factor * (DirectX::XM_2PI - abs(m_yawRemainingToTurn));
	}
}

void Player::MoveForward(bool moveForward, float speed)
{
	m_movingForward = moveForward; 
	m_movementSpeed = speed;
}

void Player::LookLeft(float angle)
{
	// Yaw must be bound [-PI, PI]
	m_yaw += angle;

	// If it is now greater than PI, just negate it
	if (m_yaw > DirectX::XM_PI)
		m_yaw *= -1.0f;
}

void Player::LookRight(float angle)
{
	// Yaw must be bound [-PI, PI]
	m_yaw -= angle;

	// If it is now less than -PI, negate it
	if (m_yaw < -DirectX::XM_PI)
		m_yaw *= -1.0f;
}