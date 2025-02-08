#include "g_graphics.hpp"
#include <cmath>
#include <algorithm> // for std::min and std::max (btw)

// draws a vertical wall slice with texture mapping and shading
void draw_wall_slice(int screenX, int wallHeight, float shade, GLuint texture,
                     float rayAngle, float rayDirX, float rayDirY, float hitDistance)
{
    if (wallHeight <= 0 || screenX < 0 || screenX >= SCREEN_WIDTH || texture == 0)
        return;

    int screenCenter = SCREEN_HEIGHT / 2;
    int wallTop = screenCenter - (wallHeight / 2);
    int wallBottom = screenCenter + (wallHeight / 2);

    // calculate hit coordinates and derive a texture coordinate
    float hitX = camerapos.x + rayDirX * hitDistance;
    float hitY = camerapos.y + rayDirY * hitDistance;
    float texX = hitX - std::floor(hitX);
    float texY = hitY - std::floor(hitY);
    float textureCoord = std::fmod(texX + texY, 1.0f);

    // combine the given shade with ambient lighting and clamp between 0 and 1
    float finalShade = std::max(0.0f, std::min(1.0f, shade + ambient_light));

    glBindTexture(GL_TEXTURE_2D, texture);
    glColor3f(finalShade, finalShade, finalShade);

    glBegin(GL_QUADS);
    const float texSliceWidth = 0.01f; // width of the texture slice
    float xPos = static_cast<float>(screenX);

    glTexCoord2f(textureCoord, 0.0f);
    glVertex2f(xPos, wallTop);

    glTexCoord2f(textureCoord + texSliceWidth, 0.0f);
    glVertex2f(xPos + 1.0f, wallTop);

    glTexCoord2f(textureCoord + texSliceWidth, 1.0f);
    glVertex2f(xPos + 1.0f, wallBottom);

    glTexCoord2f(textureCoord, 1.0f);
    glVertex2f(xPos, wallBottom);
    glEnd();
}

// renders the 3D scene using a raycasting approach
void render_scene()
{
    float startAngle = yaw - (FOV / 2.0f);
    float angleStep = FOV / NUM_RAYS;

    GLuint wallTexture = load_texture(wallsTexture);

    // enable texture mapping once at the start
    glEnable(GL_TEXTURE_2D);

    for (int rayIndex = 0, currentAngle = 0; rayIndex < NUM_RAYS; ++rayIndex)
    {
        float rayAngle = startAngle + rayIndex * angleStep;
        float rayDirX = std::cos(rayAngle);
        float rayDirY = std::sin(rayAngle);

        float t = 0.0f;
        bool hit = false;
        float hitDistance = 0.0f;

        // ray marching until a wall is hit or maximum distance is reached
        while (!hit && t < 20.0f)
        {
            t += 0.01f;
            int mapX = static_cast<int>(camerapos.x + rayDirX * t);
            int mapY = static_cast<int>(camerapos.y + rayDirY * t);

            if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
            {
                if (map[mapY][mapX] == 1)
                {
                    hit = true;
                    hitDistance = t;
                }
            }
            else
            {
                // out of bounds; exit the loop to avoid infinite iteration
                hit = true;
                hitDistance = t;
            }
        }

        if (hit)
        {
            // correct distance to prevent fisheye effect
            // TODO: didnt work
            float correctedDistance = hitDistance * std::cos(rayAngle - yaw);
            int wallHeight = static_cast<int>(SCREEN_HEIGHT / correctedDistance);
            float shade = 1.0f / (1.0f + correctedDistance * 0.1f);
            draw_wall_slice(rayIndex, wallHeight, shade, wallTexture, rayAngle, rayDirX, rayDirY, hitDistance);
        }
    }

    glDisable(GL_TEXTURE_2D);
}
