#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>

// SDL2 headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>
#include <GL/glu.h>

// engine files
#include "../Map/m_map.hpp"
#include "../Player/p_player.hpp"
#include "../Graphics/g_graphics.hpp"
#include "t_types.hpp"

// window and creation vars
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_RAYS = SCREEN_WIDTH;
#define SCREEN_TITLE "Xylon (849d05794774fdb9e6ec15fb7009e7a4b1e8f23e53b6a9fd26ca0095a58e6e05)"
#define ANTI_ALIASING 8
const size_t PATH_MAX_LENGTH = 255;

// engine vars
// nothing now btw

// global vars
int *camera_debug_mode = nullptr;
int *debug_mode_shade = nullptr;
char *other_map = nullptr;
bool party = true;

// testing vars
bool showConsole = false;
std::string inputCommand;
std::vector<std::string> history;
int historyIndex = -1;

// set the pointer to "nullptr"
TTF_Font *font = nullptr;

// player and some things
bool freezeCameraPos = false;
bool freezeYaw = false;
const float FOV = M_PI / 3.0f; // field of view
float delta_time = 0.0f;       // do not remove bitch, i am warning you

// map and ui
char defaultMapPath[PATH_MAX_LENGTH] = "maps/d1.xym";
extern char mappath[PATH_MAX_LENGTH];

// other map var
char wallsTexture[PATH_MAX_LENGTH];
char skybox[PATH_MAX_LENGTH];
char songMap[PATH_MAX_LENGTH];
char creator[PATH_MAX_LENGTH];
char description[PATH_MAX_LENGTH];
float ambient_light = 0.0f;

// cache textures
std::unordered_map<std::string, GLuint> textureCache;

// function for loading textures from file and caching them
GLuint load_texture(const char *filename)
{
    // if the texture is already in cache, return it directly
    auto it = textureCache.find(filename);
    if (it != textureCache.end())
        return it->second;

    // load image using IMG_Load function and handle any errors that occur
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface)
    {
        printf("Error loading '%s': %s\n", filename, IMG_GetError());
        printf("Using fallback...\n");
        surface = IMG_Load("resources/textures/missing.png");
    }
    if (!surface)
    {
        printf("Error loading fallback image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    printf("Loading texture from: %s\n", filename);

    // convert the surface to a consistent format (RGBA or RGB)
    SDL_PixelFormatEnum target_format = (surface->format->BytesPerPixel == 4)
                                            ? SDL_PIXELFORMAT_RGBA32
                                            : SDL_PIXELFORMAT_RGB24;
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(surface, target_format, 0);
    SDL_FreeSurface(surface);
    if (!converted)
    {
        printf("Error converting surface format: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // determine the OpenGL internal format based on the converted surface format
    GLenum internal_format = (converted->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    GLenum data_format = internal_format;

    // create a new OpenGL texture and bind it
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // load the texture data into the newly created texture object
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, converted->w, converted->h, 0,
                 data_format, GL_UNSIGNED_BYTE, converted->pixels);

    // set the texture parameters (minification, magnification, and wrapping)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // free the converted surface and store the loaded texture in the cache for future use
    SDL_FreeSurface(converted);
    textureCache[filename] = texture;
    return texture;
}

Vector3 camerapos = {5.0f, 5.0f, 0.0f};
Vector3 og_camerapos = camerapos;
float yaw = 0.0f;
float og_yaw = yaw;

// check OpenGL for errors and output for the user
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

// do not remove
int setup_projection()
{
    GLenum previous_error = glGetError();
    if (previous_error != GL_NO_ERROR)
    {
        fprintf(stderr, "Warning: Existing GL error: 0x%x\n", previous_error);
    }

    glMatrixMode(GL_PROJECTION);
    if (check_for_gl_error("Switching to projection matrix mode"))
        return 0;

    glLoadIdentity();
    if (check_for_gl_error("Loading identity matrix"))
        return 0;

    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
    if (check_for_gl_error("Setting orthographic projection"))
        return 0;

    glMatrixMode(GL_MODELVIEW);
    if (check_for_gl_error("Switching to model view matrix mode"))
        return 0;

    glLoadIdentity();
    if (check_for_gl_error("Resetting model view matrix"))
        return 0;

    return 1;
}

bool check_collision(int new_x, int new_y)
{
    // checks the collision, yeah, that is
    if (new_x < 0 || new_y < 0 || new_x >= MAP_WIDTH || new_y >= MAP_HEIGHT)
        return false;
    return map[new_y][new_x] == 1;
}

// i am lazy, so, next commit i will put in t_text.cpp
void renderText(const char *text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) // if tere was an error rendering the text
    {
        printf("Error rendering text: %s\n", TTF_GetError());
        return;
    }

    // convert the surface to RGBA32 format
    SDL_Surface *formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    if (!formatted) // if there was an error converting the surface format
    {
        printf("Error converting surface format: %s\n", SDL_GetError());
        return;
    }

    int w = formatted->w;
    int h = formatted->h;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);

    SDL_FreeSurface(formatted);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x, y);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x + w, y);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x + w, y + h);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(x, y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture);
}

// init sdl
int initialize_sdl_and_subsystems()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "Audio init failed: %s\n", Mix_GetError());
        SDL_Quit();
        return 0;
    }
    if (IMG_Init(IMG_INIT_PNG) < 0)
    {
        fprintf(stderr, "SDL Image init failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    // ttf for console and gameplay
    if (TTF_Init() == -1)
    {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        exit(1);
    }
    font = TTF_OpenFont("resources/fonts/font.ttf", 16);
    if (!font)
    {
        printf("Error initializing font: %s\n", TTF_GetError());
        exit(1);
    }
    return 1;
}

void processCommand(const std::string &command)
{
    if (command == "clear")
    {
        history.clear();
    }

    else if (command == "exit")
    {
        std::exit(0);
    }
    else if (command == "texenabled")
    {
        history.push_back("dosent work now, it will crash the engine, so i needed to remove, i will fix asap");
        // texturesEnabled = false;
    }
    else if (command == "ignorepos")
    {
        freezeCameraPos = !freezeCameraPos;
    }
    else if (command == "ignoreyaw")
    {
        freezeYaw = !freezeYaw;
    }
    else if (command == "ignoreinput")
    {
        freezeCameraPos = !freezeCameraPos;
        freezeYaw = !freezeYaw;
    }
    else if (command == "remap" && load_map(mappath) == 0)
    {
        history.push_back(std::string("Loaded map: ") + std::string(mappath));
    }
    else if (command == "resxpos")
    {
        camerapos = og_camerapos;
    }
    else if (command == "resxyaw")
    {
        yaw = og_yaw;
    }
    else if (command == "resx_posyaw")
    {
        camerapos = og_camerapos,
        yaw = og_yaw;
    }
    else if (command == "close")
    {
        showConsole = false;
    }
    else if (command == "stopsong")
    {
        Mix_HaltMusic();
    }
    /*
        Mix_Music *music = Mix_LoadMUS(songMap);
        if (!music)
        {
            fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
            cleanup_resources(window, context, nullptr);
            return EXIT_FAILURE;
        }
        Mix_PlayMusic(music, -1);
    */

    else if (command == "wmap")
    {
        history.push_back(std::string("Map: ") + std::string(mappath));
    }
    else if (command.rfind("load", 0) == 0)
    {
        std::string map_name = command.substr(5);
        if (map_name.empty())
            history.push_back("Error: No map specified for loading.");
        else
            try
            {
                history.push_back(load_map(map_name.c_str()) == 0 ? "Loaded map: " + map_name : "Failed to load map: " + map_name);
            }
            catch (const std::exception &e)
            {
                history.push_back("Error loading map: " + std::string(e.what()));
            }
            catch (...)
            {
                history.push_back("Unexpected error while loading map");
            }
    }
    else
    {
        history.push_back("Unknown command: " + command);
    }
}

void renderConsole()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(SCREEN_WIDTH, 0);
    glVertex2f(SCREEN_WIDTH, 200);
    glVertex2f(0, 200);
    glEnd();

    glColor4f(1, 1, 1, 1);

    int y = 10;
    for (int i = history.size() - 1; i >= 0 && y < 180; i--)
    {
        renderText(history[i].c_str(), 10, y);
        y += 20;
    }
    std::string line = "> " + inputCommand;
    renderText(line.c_str(), 10, y);
}

void cleanup_resources(SDL_Window *window, SDL_GLContext context, Mix_Music *music)
{
    if (music)
        Mix_FreeMusic(music);
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
    if (camera_debug_mode)
        free(camera_debug_mode);
    if (debug_mode_shade)
        free(debug_mode_shade);
    Mix_CloseAudio();
    if (other_map)
        free(other_map);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    if (!initialize_sdl_and_subsystems())
        return EXIT_FAILURE;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, ANTI_ALIASING);

    SDL_Window *window = SDL_CreateWindow(SCREEN_TITLE,
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_OPENGL);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    if (!window)
    {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        cleanup_resources(nullptr, nullptr, nullptr);
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        cleanup_resources(window, nullptr, nullptr);
        return EXIT_FAILURE;
    }

    SDL_Surface *icon_surface = IMG_Load("logo/window.png");
    if (!icon_surface)
    {
        fprintf(stderr, "Failed to load window icon: %s\n", IMG_GetError());
        cleanup_resources(window, nullptr, nullptr);
        return EXIT_FAILURE;
    }
    SDL_SetWindowIcon(window, icon_surface);
    SDL_FreeSurface(icon_surface);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
        cleanup_resources(window, nullptr, nullptr);
        return EXIT_FAILURE;
    }
    glEnable(GL_MULTISAMPLE);

    printf("Loading default map...\n");
    if (load_map(defaultMapPath) != 0)
    {
        cleanup_resources(window, context, nullptr);
        return EXIT_FAILURE;
    }

    Mix_Music *music = Mix_LoadMUS(songMap);
    if (!music)
    {
        fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
        cleanup_resources(window, context, nullptr);
        return EXIT_FAILURE;
    }
    Mix_PlayMusic(music, -1);

    setup_projection();

    constexpr int TARGET_FPS = 60;
    constexpr int FRAME_DELAY = 1000 / TARGET_FPS;
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    bool running = true;
    Uint32 last_frame_time = SDL_GetTicks();

    SDL_StartTextInput();

    while (running)
    {
        Uint32 frame_start = SDL_GetTicks();
        Uint32 current_frame = SDL_GetTicks();
        delta_time = (current_frame - last_frame_time) / 1000.0f;
        last_frame_time = current_frame;

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_F1)
                {
                    showConsole = !showConsole;
                    if (showConsole)
                    {
                        SDL_StartTextInput();
                    }
                    else
                    {
                        SDL_StopTextInput();
                    }
                }
                if (showConsole)
                {
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        history.push_back(inputCommand);
                        processCommand(inputCommand);
                        inputCommand.clear();
                    }
                    else if (event.key.keysym.sym == SDLK_BACKSPACE && !inputCommand.empty())
                    {
                        inputCommand.pop_back();
                    }
                }
            }
            else if (event.type == SDL_TEXTINPUT && showConsole)
            {
                inputCommand += event.text.text;
            }
        }

        float move_x, move_y;
        if (showConsole == false)
        {
            handle_movement(state, move_x, move_y, yaw);
        }
        float new_x = camerapos.x + move_x * PLAYER_SPEED;
        float new_y = camerapos.y + move_y * PLAYER_SPEED;
        if (!check_collision(new_x, new_y))
        {
            camerapos.x = new_x;
            camerapos.y = new_y;
        }

        if (camera_debug_mode && *camera_debug_mode)
        {
            printf("Camera: pos(%.2f, %.2f, %.2f), yaw: %.2f\n",
                   camerapos.x, camerapos.y, camerapos.z, yaw);
        }

        if (camerapos.z > 1.0f)
            camerapos.z = 1.0f;
        if (camerapos.z < -1.0f)
            camerapos.z = -1.0f;

        glClear(GL_COLOR_BUFFER_BIT);
        render_scene();

        if (showConsole)
        {
            renderConsole();
        }

        SDL_GL_SwapWindow(window);

        int frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY)
            SDL_Delay(FRAME_DELAY - frame_time);
    }

    cleanup_resources(window, context, music);
    return EXIT_SUCCESS;
}
