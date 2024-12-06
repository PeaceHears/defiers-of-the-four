#pragma once
#include <windows.h>

struct InGameData
{
    bool isSpectating = false;
    POINT playerPosition;
    POINT allyPosition;
};