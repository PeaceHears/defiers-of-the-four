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
    std::vector<std::vector<int>> map;

    bool isSpectating = false;

    sf::Vector2f position;
    sf::Vector2f allyPosition;

    int shootingRobotIndex = -1;
    sf::Vector2f fireDirection;

    friend sf::Packet& operator<<(sf::Packet& packet, const PlayerState& state)
    {
        // Serialize the size of the map
        packet << static_cast<sf::Uint32>(state.map.size());

        // Serialize each item in the map
        for (const auto& mapItem : state.map)
        {
            // Serialize the size of the inner vector
            packet << static_cast<sf::Uint32>(mapItem.size());

            // Serialize each element in the inner vector
            for (const auto& item : mapItem)
            {
                packet << item;
            }
        }

        // Serialize player robots values
        packet << state.position.x << state.position.y << state.allyPosition.x << state.allyPosition.y 
            << state.isSpectating << state.shootingRobotIndex << state.fireDirection.x << state.fireDirection.y;

        return packet;
    }

    friend sf::Packet& operator>>(sf::Packet& packet, PlayerState& state)
    {
        sf::Uint32 mapOuterSize;

        // Deserialize the outer vector size
        if (packet >> mapOuterSize)
        {
            state.map.resize(mapOuterSize);

            // Deserialize each inner vector
            for (sf::Uint32 i = 0; i < mapOuterSize; ++i)
            {
                sf::Uint32 mapInnerSize;

                if (packet >> mapInnerSize)
                {
                    state.map[i].resize(mapInnerSize);

                    // Deserialize each element in the inner vector
                    for (sf::Uint32 j = 0; j < mapInnerSize; ++j)
                    {
                        packet >> state.map[i][j];
                    }
                }
            }
        }

        packet >> state.position.x >> state.position.y >> state.allyPosition.x >> state.allyPosition.y 
            >> state.isSpectating >> state.shootingRobotIndex >> state.fireDirection.x >> state.fireDirection.y;

        return packet;
    }
};

// Client class
class GameClient
{
public:
    GameClient(const sf::IpAddress& serverAddress, unsigned short serverPort);

    void run(); // Main client loop
    void stop(); // Stop the client

    void setMapData(const std::vector<std::vector<int>>& map);
    void setInGameData(const InGameData& inGameData);
    void setBulletData(const int shootingRobotIndex = -1, const int fireDirectionX = 0, const int fireDirectionY = 0);
    void setDataMutex(std::unordered_map<std::string, PlayerState>& gameState);
    void setSpectaterInfo(const bool isSpectating);
    PlayerState getGamerPlayerState(const std::unordered_map<std::string, PlayerState>& gameState);
    PlayerState getSpectatorState(const std::unordered_map<std::string, PlayerState>& gameState);
    void deletePlayer();

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