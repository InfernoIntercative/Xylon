#pragma once

#include <cstddef>

extern int MAP_WIDTH;
extern int MAP_HEIGHT;
extern int **map;

extern const size_t PATH_MAX_LENGTH;

extern char songMap[256];
extern char wallsTexture[256];
extern char skybox[256];

extern float ambient_light;
extern char mappath[256];

int load_map(const char *filename);
