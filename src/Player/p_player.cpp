#include <cmath>
#include <stdio.h>

// headers
#include "p_player.hpp"

// constants
constexpr float EPSILON = 1e-6;
extern const float MOUSE_SENS; // not used
extern bool freezeYaw;
extern bool freezeCameraPos;

/**
 * @brief Handles the player's movement based on keyboard input.
 *
 * This function updates the movement vector (move_x, move_y) based on the
 * current state of the keyboard. It supports forward, backward, and strafe
 * movements using the W, A, S, and D keys, as well as yaw (left/right) and
 * pitch (up/down) adjustments.
 *
 * @param state Pointer to the array representing the current state of the keyboard.
 * @param move_x Reference to the float variable representing the movement in the x direction.
 * @param move_y Reference to the float variable representing the movement in the y direction.
 * @param yaw Reference to the yaw angle (rotation around the vertical axis).
 * @param pitch Reference to the pitch angle (vertical look).
 */
void handle_movement(const Uint8 *state, float &move_x, float &move_y, float &yaw)
{
    move_x = 0.0f;
    move_y = 0.0f;

    // precompute cos and sin of yaw
    float cos_yaw = cos(yaw);
    float sin_yaw = sin(yaw);

    if (!freezeCameraPos)
    {
        // Forward/Backward movement
        if (state[SDL_SCANCODE_W])
        {
            move_x += cos_yaw * PLAYER_SPEED;
            move_y += sin_yaw * PLAYER_SPEED;
        }
        if (state[SDL_SCANCODE_S])
        {
            move_x -= cos_yaw * PLAYER_SPEED;
            move_y -= sin_yaw * PLAYER_SPEED;
        }

        // strafe movement (calculate once and reuse)
        float strafe_yaw = yaw + M_PI / 2.0f;
        float cos_strafe_yaw = cos(strafe_yaw);
        float sin_strafe_yaw = sin(strafe_yaw);

        if (state[SDL_SCANCODE_A])
        {
            move_x -= cos_strafe_yaw * PLAYER_SPEED;
            move_y -= sin_strafe_yaw * PLAYER_SPEED;
        }
        if (state[SDL_SCANCODE_D])
        {
            move_x += cos_strafe_yaw * PLAYER_SPEED;
            move_y += sin_strafe_yaw * PLAYER_SPEED;
        }

        // normalize movement vector to ensure consistent speed
        float length = sqrt(move_x * move_x + move_y * move_y);
        if (length > EPSILON)
        {
            move_x = (move_x / length) * PLAYER_SPEED;
            move_y = (move_y / length) * PLAYER_SPEED;
        }
    }

    if (!freezeYaw)
    {
        int mouseXRel, mouseYRel;
        SDL_GetRelativeMouseState(&mouseXRel, &mouseYRel);
        yaw += mouseXRel * MOUSE_SENS;
    }
}
