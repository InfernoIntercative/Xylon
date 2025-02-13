#pragma once

#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include "../Core/t_types.hpp"
#include "../Map/m_map.hpp"

// external dependencies
extern int **map;
extern float yaw;                                 // player's view yaw
extern Vector3 camerapos;                         // camera position
extern GLuint load_texture(const char *filepath); // texture loading function

/*
header copying the variables from another file
*/
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int NUM_RAYS;
extern int *debug_mode;
extern int *debug_mode_shade;
extern char *other_map;
extern const float FOV;
extern float ambient_light;

extern char skybox[256];

// function declarations
void draw_wall_slice(int x, int height, float shade, GLuint texture, float ray_angle, float ray_x, float ray_y, float hit_dist);
void render_scene();
void render_skybox();
