#pragma once
#include <windows.h>

extern bool isSpectating;

struct InGameData
{
    bool isSpectating = false;
    int health = 0;
    int allyHealth = 0;
    POINT playerPosition;
    POINT allyPosition;
};

struct DemonData
{
    int id = 0;
    int baseNumber = 0;
    int health = 0;
    POINT position;
};