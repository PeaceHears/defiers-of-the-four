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

    std::vector<std::vector<int>> map;

    std::vector<DemonData> demons;

    sf::Vector2i position = sf::Vector2i(-1, -1);
    sf::Vector2i allyPosition = sf::Vector2i(-1, -1);

    int shootingRobotIndex = -1;
    sf::Vector2i fireDirection = sf::Vector2i(-1, -1);
    int shootingAllyRobotIndex = -1;
    sf::Vector2i allyFireDirection = sf::Vector2i(-1, -1);

    int health = -1;
    int allyHealth = -1;

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

        // Serialize the size of the demon positions
        packet << static_cast<sf::Uint32>(state.demons.size());

        // Serialize each item in the demon positions
        for (const auto& demon : state.demons)
        {
            sf::Vector2i demonPosition = sf::Vector2i(demon.position.x, demon.position.y);

            packet << demon.id << demon.baseNumber << demon.health << demonPosition.x << demonPosition.y;
        }

        // Serialize player robots values
        packet << state.position.x << state.position.y << state.allyPosition.x << state.allyPosition.y
            << state.isSpectating <<
            state.shootingRobotIndex << state.fireDirection.x << state.fireDirection.y <<
            state.shootingAllyRobotIndex << state.allyFireDirection.x << state.allyFireDirection.y <<
            state.health << state.allyHealth;

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

        sf::Uint32 demonPositionsOuterSize;

        if (packet >> demonPositionsOuterSize)
        {
            state.demons.resize(demonPositionsOuterSize);

            // Deserialize each item in the demon positions
            for (auto& demon : state.demons)
            {
                sf::Vector2i demonPosition = sf::Vector2i(demon.position.x, demon.position.y);

                packet >> demon.id >> demon.baseNumber >> demon.health >> demonPosition.x >> demonPosition.y;
            }
        }

        packet >> state.position.x >> state.position.y >> state.allyPosition.x >> state.allyPosition.y
            >> state.isSpectating >>
            state.shootingRobotIndex >> state.fireDirection.x >> state.fireDirection.y >>
            state.shootingAllyRobotIndex >> state.allyFireDirection.x >> state.allyFireDirection.y >>
            state.health >> state.allyHealth;

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
    void setDemonData(const std::vector<DemonData>& demons);
    void setInGameData(const InGameData& inGameData);
    void setBulletData(const int shootingRobotIndex = -1, const int fireDirectionX = 0, const int fireDirectionY = 0);
    void setAllyBulletData(const int shootingAllyRobotIndex = -1,
        const int allyFireDirectionX = 0, const int allyFireDirectionY = 0);
    void setDataMutex(std::unordered_map<std::string, PlayerState>& gameState);
    void setSpectaterInfo(const bool isSpectating);
    void setGamerPlayerState(const std::unordered_map<std::string, PlayerState>& gameState, PlayerState& playerState);
    void setSpectatorState(const std::unordered_map<std::string, PlayerState>& gameState, PlayerState& playerState);
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