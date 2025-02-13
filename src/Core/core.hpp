#pragma once

#include <stdio.h>

const size_t PATH_MAX_LENGTH = 256;

// defines super importants constants for the engine
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_RAYS = SCREEN_WIDTH;
const char SCREEN_TITLE[] = "Xylon (46ac390a5aa703337e4972822061ea66161b25e362f08974e48d1366cc08c3b0)";
const float ANTI_ALIASING = 8.0f;
const char defaultMapPath[PATH_MAX_LENGTH] = "maps/d1.xym";
