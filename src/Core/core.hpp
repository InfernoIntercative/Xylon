#pragma once

#include <stdio.h>

const size_t PATH_MAX_LENGTH = 256;

// defines super importants constants for the engine
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_RAYS = SCREEN_WIDTH;
const char SCREEN_TITLE[] = "Xylon (MAIN BRANCH)";
const float ANTI_ALIASING = 8.0f;
const char defaultLevelsPath[PATH_MAX_LENGTH] = "levels/d1.xym";
