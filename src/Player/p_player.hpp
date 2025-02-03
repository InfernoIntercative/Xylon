#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>

// define the player's speed (adjust as needed)
extern float delta_time;
#define PLAYER_SPEED 10.0f * delta_time;
#define TURN_SPEED 1.2f * delta_time;

// player's movement angles (yaw and yaw)
extern float yaw; // horizontal angle

// function to handle player movement based on key states
void handle_movement(const Uint8 *state, float &move_x, float &move_y, float &yaw);
void handle_mouse_input();

#endif // PLAYER_HPP
