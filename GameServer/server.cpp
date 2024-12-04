#include "server.h"
#include <iostream>

// Constructor: Initializes the server with the specified port
GameServer::GameServer(unsigned short port) : port(port)
{
    if (socket.bind(port) != sf::Socket::Done)
    {
        throw std::runtime_error("Failed to bind server to port " + std::to_string(port));
    }

    socket.setBlocking(false); // Non-blocking mode for smooth execution

    // Print local IP address and port
    std::cout << "Socket bound to IP: " << sf::IpAddress::getLocalAddress()
        << " and Port: " << port << std::endl;

    // Print public IP address
    std::cout << "Public IP: " << sf::IpAddress::getPublicAddress(sf::seconds(2)) << std::endl;
}

// Main server loop
void GameServer::run()
{
    while (true)
    {
        // Handle incoming packets
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short senderPort;

        if (socket.receive(packet, sender, senderPort) == sf::Socket::Done)
        {
            handleClientInput(packet, sender, senderPort);
        }

        // Broadcast the updated game state to all clients
        broadcastGameState();
    }
}

// Handle input from a specific client
void GameServer::handleClientInput(sf::Packet& packet, sf::IpAddress sender, unsigned short port)
{
    PlayerState state;

    if (packet >> state)
    {
        // Update the player's state based on their input
        PlayerState& state = players[sender];

        std::cout << "Received input from " << sender << ":" << port
            << " -> Position: (" << state.position.x << ", " << state.position.y;
    }
    else
    {
        std::cerr << "Failed to parse input from " << sender << ":" << port << std::endl;
    }
}

// Broadcast the game state to all connected clients
void GameServer::broadcastGameState()
{
    sf::Packet statePacket;

    // Pack the state of all players
    for (const auto& player : players)
    {
        const auto& ip = player.first;
        const auto& state = player.second;

        statePacket << ip.toString() << state;
    }

    // Send the state packet to all connected clients
    for (const auto& player : players)
    {
        const auto& ip = player.first;

        if (socket.send(statePacket, ip, port) != sf::Socket::Done)
        {
            std::cerr << "Failed to send state to " << ip << std::endl;
        }
    }
}