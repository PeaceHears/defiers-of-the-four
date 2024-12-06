#pragma once

#include <SFML/Network.hpp>
#include <unordered_map>
#include <string>
#include "Globals.h"
#include <atomic>
#include <mutex>

// Player state structure
struct PlayerState
{
    bool isSpectating = false;
    sf::Vector2f position;
    sf::Vector2f allyPosition;

    friend sf::Packet& operator<<(sf::Packet& packet, const PlayerState& state)
    {
        return packet << state.position.x << state.position.y << state.allyPosition.x << state.allyPosition.y 
            << state.isSpectating;
    }

    friend sf::Packet& operator>>(sf::Packet& packet, PlayerState& state)
    {
        return packet >> state.position.x >> state.position.y >> state.allyPosition.x >> state.allyPosition.y 
            >> state.isSpectating;
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
    void setDataMutex(std::unordered_map<std::string, PlayerState>& gameState);
    void setSpectaterInfo(const bool isSpectating);
    const PlayerState& getGamerPlayerState(const std::unordered_map<std::string, PlayerState>& gameState);

private:
    void sendInput();
    void receiveGameState();

    sf::UdpSocket socket;
    sf::IpAddress serverAddress;
    unsigned short serverPort;

    std::atomic<bool> running; // Flag to control client thread execution
    std::mutex dataMutex;

    std::string localPlayerId;
    PlayerState localPlayerState;
    std::unordered_map<std::string, PlayerState> sharedGameState;

    void parseGameState(sf::Packet& packet);
};