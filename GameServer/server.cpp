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

        auto receiveStatus = socket.receive(packet, sender, senderPort);

        while (receiveStatus == sf::Socket::Done)
        {
            handleClientInput(packet, sender, senderPort);

            receiveStatus = socket.receive(packet, sender, senderPort);
        }

        checkAlivePlayers();

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
        const auto& clientKey = std::make_pair(sender, senderPort);

        // Update the player's state based on their input
        players[clientKey] = state;

        lastActiveTime[clientKey] = std::chrono::steady_clock::now();

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

    //Check bullet state
    // 
    // TODO: This can be the cause of resetting PlayerState

    for (const auto& player : players)
    {
        const auto& previousState = player.second;

        if (previousState.shootingRobotIndex == -2)
        {
            for (const auto& player : players)
            {
                const auto& client = player.first;
                auto state = player.second;
                const auto& clientKey = std::make_pair(client.first, client.second);

                state.shootingRobotIndex = -1;

                players[clientKey] = state;
            }
        }

        if (previousState.shootingAllyRobotIndex == -2)
        {
            for (const auto& player : players)
            {
                const auto& client = player.first;
                auto state = player.second;
                const auto& clientKey = std::make_pair(client.first, client.second);

                state.shootingAllyRobotIndex = -1;

                players[clientKey] = state;
            }
        }
    }

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

void GameServer::checkAlivePlayers()
{
    auto now = std::chrono::steady_clock::now();
    const auto timeoutThreshold = std::chrono::seconds(2);

    for (auto it = lastActiveTime.begin(); it != lastActiveTime.end();)
    {
        if (now - it->second > timeoutThreshold)
        {
            const auto& clientKey = it->first;
            std::cout << "Client " << clientKey.first << ":" << clientKey.second << " timed out.\n";

            // Remove the timed-out client
            players.erase(clientKey);
            it = lastActiveTime.erase(it);
        }
        else
        {
            ++it;
        }
    }
}