#include "ClientRobotInterpolation.h"

ClientRobotInterpolation::ClientRobotInterpolation()
{

}

ClientRobotInterpolation::~ClientRobotInterpolation()
{

}

void ClientRobotInterpolation::InterpolatePosition(const float deltaTime)
{
    // Increment interpolation time
    interpolationTime += deltaTime;

    // Calculate alpha (normalized time between updates)
    float alpha = std::min(interpolationTime / updateInterval, 1.0f);

    // Linear interpolation between previous and target positions
    currentPosition = Lerp(previousPosition, targetPosition, alpha);
}

void ClientRobotInterpolation::OnServerUpdate(const sf::Vector2i& newPosition)
{
    // When new data arrives from the server, reset interpolation
    previousPosition = currentPosition;
    targetPosition = newPosition;
    interpolationTime = 0.0f;
}

const sf::Vector2i& ClientRobotInterpolation::Lerp(const sf::Vector2i& start, const sf::Vector2i& end, const float alpha) const
{
    const int lastX = end.x - start.x;
    const int lastY = end.y - start.y;

    return sf::Vector2i(start.x + alpha * lastX, start.y + alpha * lastY);
}