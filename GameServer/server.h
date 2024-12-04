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

// Hash function for sf::IpAddress to use it as a key in unordered_map
struct IpAddressHash
{
    std::size_t operator()(const sf::IpAddress& ip) const
    {
        return std::hash<std::string>()(ip.toString());
    }
};

// Server class
class GameServer
{
public:
    GameServer(unsigned short port);
    void run();

private:
    void handleClientInput(sf::Packet& packet, sf::IpAddress sender, unsigned short port);
    void broadcastGameState();

    sf::UdpSocket socket;
    unsigned short port;
    std::unordered_map<sf::IpAddress, PlayerState, IpAddressHash> players; // Player states
};