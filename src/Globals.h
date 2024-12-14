#pragma once
#include <windows.h>

extern bool isSpectating;

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
    POINT position;
};