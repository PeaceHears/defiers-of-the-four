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
    sf::Clock clock;

    while (true) 
    {
        sf::Time elapsed = clock.restart();

        // Handle incoming packets
        sf::Packet packet;
        sf::IpAddress sender;
        unsigned short senderPort;

        while (socket.receive(packet, sender, senderPort) == sf::Socket::Done) 
        {
            handleClientInput(packet, sender, senderPort);
        }

        // Broadcast the updated game state to all clients
        broadcastGameState();

        // Avoid busy-waiting
        // Limit the update rate
        sf::Time frameTime = sf::milliseconds(16); // ~60 updates per second

        if (elapsed < frameTime) 
        {
            sf::sleep(frameTime - elapsed);
        }
    }
}

// Handle input from a specific client
void GameServer::handleClientInput(sf::Packet& packet, sf::IpAddress sender, unsigned short senderPort) 
{
    PlayerState state;

    if (packet >> state)
    {
        // Update the player's state based on their input
        players[{sender, senderPort}] = state;

        std::cout << "Received input from " << sender << ":" << senderPort
            << " -> Position: (" << state.position.x << ", " << state.position.y << ")"
            << " -> Ally Position: (" << state.allyPosition.x << ", " << state.allyPosition.y << ")" << std::endl;
    }
    else 
    {
        std::cerr << "Failed to parse input from " << sender << ":" << senderPort << std::endl;
    }
}

// Broadcast the game state to all connected clients
void GameServer::broadcastGameState() 
{
    sf::Packet statePacket;

    // Pack the state of all players
    for (const auto& player : players)
    {
        const auto& client = player.first;
        const auto& state = player.second;

        statePacket << client.first.toString() << client.second << state;
    }

    for (const auto& player : players) 
    {
        const auto& client = player.first;

        std::cout << "SEND -> IP & Port: " << client.first.toString() << ":" << client.second << std::endl;
        
        // Send the state packet to the specific client
        if (socket.send(statePacket, client.first, client.second) != sf::Socket::Done) 
        {
            std::cerr << "Failed to send state to " << client.first << ":" << client.second << std::endl;
        }
    }
}
