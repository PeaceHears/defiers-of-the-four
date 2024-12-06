#pragma once

#include <SFML/Network.hpp>
#include <unordered_map>
#include <string>

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
    void handleClientInput(sf::Packet& packet, sf::IpAddress sender, unsigned short senderPort);
    void broadcastGameState();

    sf::UdpSocket socket;
    unsigned short port;
    //std::unordered_map<sf::IpAddress, PlayerState, IpAddressHash> players; // Player states
    std::unordered_map<std::pair<sf::IpAddress, unsigned short>, PlayerState, PairHash> players; // Player states
   
};