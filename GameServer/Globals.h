#pragma once
#include <windows.h>

struct InGameData
{
    bool isSpectating = false;
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