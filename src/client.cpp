#include "client.h"
#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>

// Constructor: Initializes the client with server details
GameClient::GameClient(const sf::IpAddress & serverAddress, unsigned short serverPort)
    : serverAddress(serverAddress), serverPort(serverPort), running(true)
{
    if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) 
    {
        std::cerr << "Failed to bind client socket to a port." << std::endl;
    }

    socket.setBlocking(false); // Non-blocking mode
}

// Main client loop
void GameClient::run()
{
    std::cout << "Client started. Press Ctrl+C to exit." << std::endl;

    while (running) 
    {
        sendInput();        // Send player input to the server
        receiveGameState(); // Receive updated game state from the server

        //Simulate game update/rendering logic here
        std::cout << "Game state received: " << gameState.size() << " players active." << std::endl;

        //Add a small delay to prevent high CPU usage
        //std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        sf::sleep(sf::milliseconds(16));
    }
}

void GameClient::stop()
{
    running = false;
}

// Send player input to the server
void GameClient::sendInput()
{
    // Send the input to the server
    sf::Packet packet;
    packet << localPlayerState;

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

    sf::Socket::Status status = socket.receive(packet, sender, senderPort);

    if (status == sf::Socket::Done) 
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
    else if (status == sf::Socket::NotReady)
    {
        // No data received yet
        std::cerr << "Server is not ready. No data received yet." << std::endl;
    }
    else 
    {
        std::cerr << "Error receiving data from server." << std::endl;
    }
}

void GameClient::setInGameData(const InGameData& inGameData)
{
    localPlayerState.position = sf::Vector2f(inGameData.playerPosition.x, inGameData.playerPosition.y);
    localPlayerState.allyPosition = sf::Vector2f(inGameData.allyPosition.x, inGameData.allyPosition.y);
}