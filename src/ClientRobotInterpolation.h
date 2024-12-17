#pragma once

#include <SFML/Network.hpp>

class ClientRobotInterpolation
{
public:
	ClientRobotInterpolation();
	~ClientRobotInterpolation();

	void InterpolatePosition(const float deltaTime);
	void OnServerUpdate(const sf::Vector2i& newPosition);

	void SetCurrentPosition(const sf::Vector2i& _currentPosition) { currentPosition = _currentPosition; }
	void SetTargetPosition(const sf::Vector2i& _targetPosition) { targetPosition = _targetPosition; }

	const sf::Vector2i& GetCurrentPosition() const { return currentPosition; }

private:
	sf::Vector2i currentPosition;
	sf::Vector2i previousPosition;
	sf::Vector2i targetPosition;

	float interpolationTime = 0.0f;
	float updateInterval = 0.1f;  // Server updates every 0.1s

	const sf::Vector2i& Lerp(const sf::Vector2i& start, const sf::Vector2i& end, const float alpha) const;
};

