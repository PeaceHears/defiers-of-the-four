#pragma once

#include <SFML/Network.hpp>
#include <unordered_map>
#include <string>
#include "Globals.h"
#include <atomic>

// Player state structure
struct PlayerState
{
    sf::Vector2f position;
    sf::Vector2f allyPosition;

    friend sf::Packet& operator<<(sf::Packet& packet, const PlayerState& state)
    {
        return packet << state.position.x << state.position.y << state.allyPosition.x << state.allyPosition.y;
    }

    friend sf::Packet& operator>>(sf::Packet& packet, PlayerState& state)
    {
        return packet >> state.position.x >> state.position.y >> state.allyPosition.x >> state.allyPosition.y;
    }
};

// Client class
class GameClient
{
public:
    GameClient(const sf::IpAddress& serverAddress, unsigned short serverPort);
    void run(); // Main client loop
    void stop(); // Stop the client
    void setInGameData(const InGameData& inGameData);

private:
    void sendInput();
    void receiveGameState();

    sf::UdpSocket socket;
    sf::IpAddress serverAddress;
    unsigned short serverPort;

    std::atomic<bool> running; // Flag to control client thread execution

    PlayerState localPlayerState;
    std::unordered_map<std::string, PlayerState> gameState;
};