#pragma once

#include <SFML/Network.hpp>
#include <unordered_map>
#include <string>
#include <__msvc_chrono.hpp>
#include "Globals.h"

// Type for tracking the last received time of each client
using TimePoint = std::chrono::steady_clock::time_point;

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

namespace std
{
    template <>
    struct hash<sf::IpAddress> 
    {
        std::size_t operator()(const sf::IpAddress& ip) const
        {
            return std::hash<std::string>()(ip.toString());
        }
    };
}

// Hash function for sf::IpAddress to use it as a key in unordered_map
struct IpAddressHash
{
    std::size_t operator()(const sf::IpAddress& ip) const
    {
        return std::hash<std::string>()(ip.toString());
    }
};

struct PairHash 
{
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const 
    {
        std::size_t h1 = std::hash<T1>{}(pair.first);  // Hash the first element
        std::size_t h2 = std::hash<T2>{}(pair.second); // Hash the second element

        // Combine the hashes (using XOR and bit shifting for better distribution)
        return h1 ^ (h2 << 1);
    }
};

// Server class
class GameServer
{
public:
    GameServer(unsigned short port);
    void run();

private:
    sf::UdpSocket socket;
    unsigned short port;
    //std::unordered_map<sf::IpAddress, PlayerState, IpAddressHash> players; // Player states
    std::unordered_map<std::pair<sf::IpAddress, unsigned short>, PlayerState, PairHash> players; // Player states
    std::unordered_map<std::pair<sf::IpAddress, unsigned short>, TimePoint, PairHash> lastActiveTime;

    void handleClientInput(sf::Packet& packet, sf::IpAddress sender, unsigned short senderPort);
    void broadcastGameState();
    void checkAlivePlayers();
};