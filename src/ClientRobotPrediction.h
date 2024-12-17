#pragma once

#include <SFML/Network.hpp>

class ClientRobotPrediction
{
public:
	ClientRobotPrediction();
	~ClientRobotPrediction();

	void PredictPosition(const float deltaTime);
	void OnServerUpdate(const sf::Vector2i& serverPosition, const sf::Vector2i& serverVelocity);

	void SetVelocity(const sf::Vector2i& _velocity) { velocity = _velocity; }
	const sf::Vector2i& GetPredictedPosition() const { return predictedPosition; }

private:
	sf::Vector2i predictedPosition;
	sf::Vector2i lastPosition;
	sf::Vector2i velocity;  // Movement speed in X, Y direction
	sf::Vector2i lastKnownServerPosition;

	const sf::Vector2i& ScaleVector(const sf::Vector2i& vector, const float scalar) const;
};

