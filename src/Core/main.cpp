#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <string>
#include <array>

// sdl2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <GL/gl.h>

// engine files
#include "../Map/m_map.hpp"
#include "../Player/p_player.hpp"
#include "../Graphics/g_graphics.hpp"
#include "t_types.hpp"

// engine constants (because magic numbers are bad, apparently)
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
int NUM_RAYS = SCREEN_WIDTH;
#define SCREEN_TITLE "Xylon (da3b0cfca713b04bce7269995be822f747e51be7c913dbc063934fe46a7bb990)"
#define ANTI_ALIASING 8
const size_t PATH_MAX_LENGTH = 255;

// pointers (because we need debug)
int *camera_debug_mode = NULL;
int *debug_mode_shade = NULL;
char *other_map = NULL;

// player constants
const float FOV = (M_PI / 3); // field of view, aka how much we pretend to see
float delta_time = 0.0f;      /*
don't remove, for higher FPS, the game for some reason, will be on higher speed
*/

// map constants
// before loading map
char defaultMapPath[PATH_MAX_LENGTH] = "maps/map.iemf";

// in map constants
char wallsTexture[PATH_MAX_LENGTH];
char skybox[PATH_MAX_LENGTH];
char songMap[PATH_MAX_LENGTH];
char creator[PATH_MAX_LENGTH];
char description[PATH_MAX_LENGTH];

float ambient_light = 0.0f;

std::unordered_map<std::string, GLuint> textureCache;

GLuint load_texture(const char *filename)
{
    auto it = textureCache.find(filename);
    if (it != textureCache.end())
    {
        return it->second;
    }

    SDL_Surface *surface = IMG_Load(filename);
    if (!surface)
    {
        printf("Error trying to load the image: %s\n", IMG_GetError());
        printf("Trying to use fallback...\n");
        surface = IMG_Load("resources/textures/missing.png");
    }

    if (!surface)
    {
        printf("Error loading fallback image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    printf("Loading texture from: %s\n", filename);

    /*
        convert the surface for a consistent format (RGBA32/RGB24)
    */
    SDL_PixelFormatEnum target_format = surface->format->BytesPerPixel == 4
                                            ? SDL_PIXELFORMAT_RGBA32
                                            : SDL_PIXELFORMAT_RGB24;

    SDL_Surface *converted = SDL_ConvertSurfaceFormat(surface, target_format, 0);
    SDL_FreeSurface(surface); // release the original surface
    surface = converted;

    if (!surface)
    {
        printf("Error converting surface format: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // determines OpenGL formats
    GLenum internal_format, data_format;
    if (surface->format->BytesPerPixel == 4)
    {
        internal_format = GL_RGBA;
        data_format = GL_RGBA;
    }
    else
    {
        internal_format = GL_RGB;
        data_format = GL_RGB;
    }

    // generate OpenGL texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // resolves texture alignment issues
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // send texture data
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        surface->w,
        surface->h,
        0,
        data_format,
        GL_UNSIGNED_BYTE,
        surface->pixels);

    // define the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // release the surface
    SDL_FreeSurface(surface);

    // store texture in cache (don't change this, or i will chase your family)
    textureCache[filename] = texture;

    return texture;
}

Vector3 camera_pos = {5.0f, 5.0f, 0.0f};
float yaw = 0.0f;
/*
20:19 - not used, because of some things, will be on the Engine later
00:04 - nevermind
03:08 - fuck it, i am WILL be adding far later!
*/
// float pitch = 0.0f;

// it is used on setup_projection
bool check_for_gl_error(const char *error_message)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        fprintf(stderr, "Error: %s - 0x%x\n", error_message, error);
        return true;
    }
    return false;
}

/*
sets up the OpenGL projection matrix for 2.5D rendering
returns: 1 on success, 0 on failure.
*/
int setup_projection()
{
    // save previous error state
    GLenum previous_error = glGetError();
    if (previous_error != GL_NO_ERROR)
    {
        fprintf(stderr, "Warning: Existing OpenGL error state: 0x%x\n", previous_error);
    }

    // switch to projection matrix mode and handle potential errors
    glMatrixMode(GL_PROJECTION);
    if (check_for_gl_error("Failed to switch to projection matrix mode"))
        return 0;

    // reset the projection matrix
    glLoadIdentity();
    if (check_for_gl_error("Failed to load identity matrix"))
        return 0;

    // set up orthographic projection with specified parameters
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
    if (check_for_gl_error("Failed to set orthographic projection"))
        return 0;

    // switch back to model view matrix for rendering
    glMatrixMode(GL_MODELVIEW);
    if (check_for_gl_error("Failed to switch to model view matrix mode"))
        return 0;

    // reset the model view matrix
    glLoadIdentity();
    if (check_for_gl_error("Failed to reset model view matrix"))
        return 0;

    return 1;
}

/**
 * @brief Checks if there is a collision at the specified coordinates
 *
 * Takes floating point coordinates and checks if there is a collision
 * by converting them to integer map coordinates and checking the map array.
 * Performs bounds checking to ensure coordinates are within map limits.
 *
 * @param new_x The x-coordinate to check for collision
 * @param new_y The y-coordinate to check for collision
 * @return int Returns 1 if there is a collision, 0 if position is clear or out of bounds
 */
bool check_collision(int new_x, int new_y)
{
    if (new_x < 0 || new_y < 0 || new_x >= MAP_WIDTH || new_y >= MAP_HEIGHT)
    {
        return false; // Out of bounds, no collision
    }

    return map[new_y][new_x] == 1; // Check for collision at the given position
}

// debug mode (because we're not perfect, yet =P)
int is_camera_debug_mode(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--camera-debug") == 0)
        {
            // allocate memory for the debug mode flag
            camera_debug_mode = (int *)malloc(sizeof(int));
            if (camera_debug_mode == NULL)
            {
                fprintf(stderr, "Failed to allocate memory for debug mode.\n");
                exit(EXIT_FAILURE);
            }
            *camera_debug_mode = 1;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Function declaration for loading custom maps (Development in Progress)
 *
 * This functionality allows loading alternative maps for debugging and testing purposes.
 * Currently under development and not fully implemented.
 *
 * @see map_load_for_other_files for the implementation
 * @warning Not functional in current version
 */
int map_load_for_other_files(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--map") == 0)
        {
            if (i + 1 < argc)
            {                                    // ensure there's a file name after --map
                other_map = strdup(argv[i + 1]); // duplicate the map filename
                if (other_map == NULL)
                {
                    fprintf(stderr, "Failed to allocate memory for the map filename.\n");
                    exit(EXIT_FAILURE);
                }
                return 1;
            }
            else
            {
                fprintf(stderr, "Error: No map file specified after --map.\n");
                fprintf(stderr, "Oops!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

int initialize_sdl_and_subsystems()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "Failed to initialize audio: %s\n", Mix_GetError());
        SDL_Quit();
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) < 0)
    {
        fprintf(stderr, "SDL Image Init failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    return 1;
}

void cleanup_resources(SDL_Window *window, SDL_GLContext context, Mix_Music *music)
{
    if (music)
    {
        Mix_FreeMusic(music);
    }
    if (context)
    {
        SDL_GL_DeleteContext(context);
    }

    if (window)
    {
        SDL_DestroyWindow(window);
    }
    if (camera_debug_mode)
    {
        free(camera_debug_mode);
    }
    if (debug_mode_shade)
    {
        free(debug_mode_shade);
    }
    Mix_CloseAudio();
    if (other_map)
    {
        free(other_map);
    }
    SDL_Quit();
}

/**
 * @brief The main entry point of the application.
 *
 * This function initializes SDL and its subsystems, creates an SDL window and OpenGL context,
 * loads the map and music, sets up the projection, and enters the main game loop.
 * The game loop handles events, updates the camera position, and renders the scene.
 * If any initialization step fails, the function cleans up resources and returns EXIT_FAILURE.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Returns EXIT_SUCCESS on successful execution, or EXIT_FAILURE on error.
 */
int main(int argc, char *argv[])
{
    if (!initialize_sdl_and_subsystems())
    {
        return EXIT_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, ANTI_ALIASING);

    SDL_Window *window = SDL_CreateWindow(SCREEN_TITLE,
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

    if (!window)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        cleanup_resources(NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    SDL_Surface *icon_surface = IMG_Load("logo/window.png");

    if (!icon_surface)
    {
        fprintf(stderr, "Failed to load window icon: %s\n", IMG_GetError()); // or use stbi_failure_reason()
        cleanup_resources(window, NULL, NULL);
        return EXIT_FAILURE;
    }

    SDL_SetWindowIcon(window, icon_surface);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
        cleanup_resources(window, NULL, NULL);
        return EXIT_FAILURE;
    }

    glEnable(GL_MULTISAMPLE);

    // handle map loading
    if (map_load_for_other_files(argc, argv))
    {
        printf("Map file loaded: %s\n", other_map);
        free(other_map);
    }
    else
    {
        printf("Loading default map...\n");
        if (load_map(defaultMapPath) != 0)
        {
            cleanup_resources(window, context, NULL);
            return EXIT_FAILURE;
        }
    }

    // initialize music
    Mix_Music *music = Mix_LoadMUS(songMap);
    if (!music)
    {
        fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
        cleanup_resources(window, context, NULL);
        return EXIT_FAILURE;
    }
    Mix_PlayMusic(music, -1);

    setup_projection();
    is_camera_debug_mode(argc, argv);
    // is_debug_mode_shade(argc, argv); (bye bye)

    // Main game loop configuration
    static constexpr int TARGET_FPS = 60;
    static constexpr int FRAME_DELAY = 1000 / TARGET_FPS;
    const Uint8 *const state = SDL_GetKeyboardState(NULL);
    bool running = true;

    Uint32 last_frame_time = SDL_GetTicks();

    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();
        Uint32 current_frame_time = SDL_GetTicks();
        delta_time = (current_frame_time - last_frame_time) / 1000.0f; // Convert to seconds
        last_frame_time = current_frame_time;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // check if camera_debug_mode is enabled and the camera_debug_mode pointer is valid
        if (camera_debug_mode && *camera_debug_mode)
        {
            /*
            print camera position (x, y, z) and yaw angle to console
            format: "Camera: pos(x.xx, y.yy, z.zz), yaw: p.pp"
            values are formatted to 2 decimal places for readability
            */
            printf("Camera: pos(%.2f, %.2f, %.2f), yaw: %.2f\n",
                   camera_pos.x, camera_pos.y, camera_pos.z, yaw);
        }

        // handle movement
        float move_x, move_y;
        handle_movement(state, move_x, move_y, yaw);

        // update position
        float new_x = camera_pos.x + move_x * PLAYER_SPEED;
        float new_y = camera_pos.y + move_y * PLAYER_SPEED;

        if (!check_collision(new_x, new_y))
        {
            camera_pos.x = new_x;
            camera_pos.y = new_y;
        }

        // clamp vertical look angle to prevent over-rotation
        if (camera_pos.z > 1.0f)
            camera_pos.z = 1.0f;
        if (camera_pos.z < -1.0f)
            camera_pos.z = -1.0f;
        // render frame
        glClear(GL_COLOR_BUFFER_BIT);
        render_scene();
        SDL_GL_SwapWindow(window);

        // frame timing
        int frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    cleanup_resources(window, context, music);
    return EXIT_SUCCESS;
}
