#pragma once
#include "pch.h" 
bool isPlayerValid(uintptr_t playerPtr);
bool isPlayerDead(uintptr_t playerPtr);
bool isEnemy(uintptr_t localPlayerPtr, uintptr_t playerPtr);
void setLocalPlayerPosition(uintptr_t localPlayerPtr, float x, float y, float z);