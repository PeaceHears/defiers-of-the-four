#include "ClientRobotPrediction.h"

ClientRobotPrediction::ClientRobotPrediction()
{

}

ClientRobotPrediction::~ClientRobotPrediction()
{

}

void ClientRobotPrediction::PredictPosition(const float deltaTime)
{
    // Scale velocity by deltaTime
    const auto& deltaPosition = ScaleVector(velocity, deltaTime);

    // Predict the new position
    predictedPosition = lastPosition + deltaPosition;
}

void ClientRobotPrediction::OnServerUpdate(const sf::Vector2i& serverPosition, const sf::Vector2i& serverVelocity)
{
    // Update position and velocity when data arrives
    lastKnownServerPosition = serverPosition;
    lastPosition = serverPosition;  // Reset position to server's authority
    velocity = serverVelocity;
}

const sf::Vector2i& ClientRobotPrediction::ScaleVector(const sf::Vector2i& vector, const float scalar) const
{
    return sf::Vector2i(vector.x * scalar, vector.y * scalar);
}