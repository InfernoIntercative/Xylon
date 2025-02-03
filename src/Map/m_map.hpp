#ifndef MAP_H
#define MAP_H

#include <cstddef>

extern int MAP_WIDTH;
extern int MAP_HEIGHT;
extern int **map;

extern const size_t PATH_MAX_LENGTH;

extern char songMap[255];
extern char wallsTexture[255];
extern char skybox[255];

extern float ambient_light;

int load_map(const char *filename);

#endif // MAP_H
