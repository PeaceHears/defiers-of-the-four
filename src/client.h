#pragma once

#include <SFML/Network.hpp>
#include <unordered_map>
#include <string>

// Player state structure
struct PlayerState
{
    sf::Vector2f position;

    friend sf::Packet& operator<<(sf::Packet& packet, const PlayerState& state)
    {
        return packet << state.position.x << state.position.y;
    }

    friend sf::Packet& operator>>(sf::Packet& packet, PlayerState& state)
    {
        return packet >> state.position.x >> state.position.y;
    }
};

// Client class
class GameClient
{
public:
    GameClient(const sf::IpAddress& serverAddress, unsigned short serverPort);
    void run();

private:
    void sendInput();
    void receiveGameState();

    sf::UdpSocket socket;
    sf::IpAddress serverAddress;
    unsigned short serverPort;

    PlayerState localPlayerState;
    std::unordered_map<std::string, PlayerState> gameState;
};