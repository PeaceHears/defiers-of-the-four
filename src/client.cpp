﻿#include "client.h"
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
        std::cout << "Game state received: " << sharedGameState.size() << " players active." << std::endl;

        //Add a small delay to prevent high CPU usage
        //std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        sf::sleep(sf::milliseconds(16)); // ~60 FPS
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

    while (status == sf::Socket::Done)
    {
        if (sender == serverAddress && senderPort == serverPort)
        {
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                localPlayerId = serverAddress.toString() + ":" + std::to_string(socket.getLocalPort());
            }

            parseGameState(packet);
        }

        const auto roundTripTime = pingClock.getElapsedTime();  // Measure round-trip time
        const auto ping = roundTripTime.asMilliseconds();
        std::cout << "Ping: " << ping << "ms" << std::endl;
        pingClock.restart();
        pingClockForCheck.restart();
        isOnLag = false;

        if (isSpectating)
        {
            sf::sleep(sf::milliseconds(ms));
        }

        status = socket.receive(packet, sender, senderPort);
    }

    if (status == sf::Socket::NotReady)
    {
        // No data received yet
        std::cerr << "Server is not ready. No data received yet." << std::endl;
    }
    else 
    {
        std::cerr << "Error receiving data from server." << std::endl;
    }
}

void GameClient::parseGameState(sf::Packet& packet)
{
    std::unordered_map<std::string, PlayerState> gameState;

    while (!packet.endOfPacket())
    {
        std::string ipAddress;
        unsigned short port;
        PlayerState state;

        packet >> ipAddress >> port >> state;

        std::string playerId = ipAddress + ":" + std::to_string(port);

        gameState[playerId] = state;

        std::string spectaterInfo = state.isSpectating ? "true" : "false";

        std::cout << "IP & Port: " << playerId << " Is Spectating?: " << spectaterInfo << std::endl;
    }

    // Lock the mutex and update the shared data
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        sharedGameState = gameState;

        for (const auto& gameData : gameState)
        {
            if (localPlayerId == gameData.first)
            {
                // To fix unnecessary shoots
                localPlayerState.shootingRobotIndex = gameData.second.shootingRobotIndex;
                localPlayerState.shootingAllyRobotIndex = gameData.second.shootingAllyRobotIndex;
            }
        }
    }
}

const bool GameClient::checkLagOnServer(const float msLimit)
{
    const auto roundTripTime = pingClockForCheck.getElapsedTime();
    const auto ping = roundTripTime.asMilliseconds();
    const bool isThereLag = ping > msLimit;

    if (isThereLag)
    {
        isOnLag = isThereLag;
        std::cout << "LAG! Ping: " << ping << "ms" << std::endl;
        pingClockForCheck.restart();
        return true;
    }

    return false;
}

void GameClient::setMapData(const std::vector<std::vector<int>>& map)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.map = map;
    }
}

void GameClient::setDemonData(const std::vector<DemonData>& demons)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.demons = demons;
    }
}

void GameClient::setInGameData(const InGameData& inGameData)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.isSpectating = inGameData.isSpectating;
        localPlayerState.health = inGameData.health;
        localPlayerState.allyHealth = inGameData.allyHealth;
        localPlayerState.position = sf::Vector2i(inGameData.playerPosition.x, inGameData.playerPosition.y);
        localPlayerState.allyPosition = sf::Vector2i(inGameData.allyPosition.x, inGameData.allyPosition.y);
        localPlayerState.velocity = sf::Vector2i(inGameData.velocity.x, inGameData.velocity.y);
        localPlayerState.allyVelocity = sf::Vector2i(inGameData.allyVelocity.x, inGameData.allyVelocity.y);
    }
}

void GameClient::setBulletData(const int shootingRobotIndex, const int fireDirectionX, const int fireDirectionY)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.shootingRobotIndex = shootingRobotIndex;
        localPlayerState.fireDirection = sf::Vector2i(fireDirectionX, fireDirectionY);
    }
}

void GameClient::setAllyBulletData(const int shootingAllyRobotIndex, const int allyFireDirectionX, const int allyFireDirectionY)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.shootingAllyRobotIndex = shootingAllyRobotIndex;
        localPlayerState.allyFireDirection = sf::Vector2i(allyFireDirectionX, allyFireDirectionY);
    }
}

void GameClient::setDataMutex(std::unordered_map<std::string, PlayerState>& gameState)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        gameState = sharedGameState;
    }
}

void GameClient::setSpectaterInfo(const bool isSpectating)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        localPlayerState.isSpectating = isSpectating;
    }
}

void GameClient::setGamerPlayerState(const std::unordered_map<std::string, PlayerState>& gameState, PlayerState& playerState)
{
    {
        std::lock_guard<std::mutex> lock(dataMutex);

        for (const auto& gameData : gameState)
        {
            const auto& playerId = gameData.first;
            const auto& playerData = gameData.second;

            if (playerId != localPlayerId && !playerData.isSpectating)
            {
                playerState = playerData;
                break;
            }
        }
    }
}