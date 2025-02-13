#include <stdio.h>
#include <GL/gl.h>
#include "../Texts/t_text.hpp"
#include "../Core/core.hpp"
#include "../Player/p_player.hpp"
#include "../Map/m_map.hpp"
#include "c_console.hpp"

#include <string>
#include <vector>

#include <SDL2/SDL_mixer.h>

bool showConsole = false;
std::string inputCommand;
std::vector<std::string> history;
int historyIndex = -1;

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
        history.push_back(std::string("Loaded map: ") + mappath);
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
