#include "client.h"
#include <iostream>
#include <unordered_map>

// Constructor: Initializes the client with server details
GameClient::GameClient(const sf::IpAddress & serverAddress, unsigned short serverPort)
    : serverAddress(serverAddress), serverPort(serverPort)
{
    socket.setBlocking(false); // Non-blocking mode
}

// Main client loop
void GameClient::run()
{
    // Handle game logic
    sendInput();          // Send player input to the server
    receiveGameState();   // Receive updated game state from the server
}

// Send player input to the server
void GameClient::sendInput()
{
    PlayerState state;

    // Send the input to the server
    sf::Packet packet;
    packet << state;

    if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done)
    {
        std::cerr << "Failed to send input to server." << std::endl;
    }
}

// Receive updated game state from the server
void GameClient::receiveGameState()
{
    sf::Packet packet;
    sf::IpAddress sender;
    unsigned short senderPort;

    if (socket.receive(packet, sender, senderPort) == sf::Socket::Done)
    {
        if (sender == serverAddress && senderPort == serverPort)
        {
            // Parse game state
            gameState.clear();

            while (!packet.endOfPacket())
            {
                std::string playerId;
                PlayerState state;
                packet >> playerId >> state;
                gameState[playerId] = state;
            }
        }
    }
}