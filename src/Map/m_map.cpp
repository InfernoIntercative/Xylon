#include "m_map.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **map = NULL;
int MAP_WIDTH = 0;
int MAP_HEIGHT = 0;

/*
must match the header, if there is a warning, there is history
*/
extern char songMap[256];
extern char creator[256];
extern char description[256];
extern char skybox[256];
extern float ambient_light;
extern char wallsTexture[256];
char mappath[256] = {0};

int load_map(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Failed to open map file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    strncpy(mappath, filename, sizeof(mappath) - 1);
    mappath[sizeof(mappath) - 1] = '\0';

    char line[256];
    float map_version = 0;
    float supported_version = 4.2;
    /*
    initializing an array to a single null character is more efficient than initializing it with all zeros.
    */
    char branch[9] = {'\0'};
    int map_section = 0;

    int temp_map_width = 0;
    int temp_map_height = 0;
    int current_row = 0;

    // first pass: determine dimensions and metadata
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if (strstr(line, "mapversion=") != NULL)
        {
            sscanf(line, "mapversion =%f", &map_version);
            if (map_version != supported_version)
            {
                printf("Unsupported map version: %f\n", map_version);
                fclose(file);
                return -1;
            }
        }
        else if (strstr(line, "branch=") != NULL)
        {
            sscanf(line, "branch=%s", branch);
        }
        else if (strstr(line, "mapsong=") != NULL)
        {
            sscanf(line, "    mapsong= %*c%[^\"]%*c", songMap);
        }
        else if (strstr(line, "creator=") != NULL)
        {
            sscanf(line, "    creator= %s", creator);
        }
        else if (strstr(line, "description=") != NULL)
        {
            sscanf(line, "    description= %*c%[^\"]%*c", description);
        }
        else if (strstr(line, "ambient_light=") != NULL)
        {
            sscanf(line, "    ambient_light= %f", &ambient_light);
        }
        else if (strstr(line, "[MAP]") != NULL)
        {
            map_section = 1;
            continue;
        }
        else if (strstr(line, "[TEXTURES]") != NULL)
        {
            break;
        }

        if (map_section && strlen(line) > 0 && line[0] != '[')
        {
            // use a temporary copy so strtok doesn't alter the original line.
            char tempLine[256];
            strcpy(tempLine, line);
            if (temp_map_width == 0)
            {
                char *token = strtok(tempLine, " ");
                while (token != NULL)
                {
                    temp_map_width++;
                    token = strtok(NULL, " ");
                }
            }
            temp_map_height++;
        }
    }

    if (temp_map_height == 0 || temp_map_width == 0)
    {
        printf("Invalid map dimensions.\n");
        fclose(file);
        return -1;
    }

    // save to global MAP_WIDTH and MAP_HEIGHT
    MAP_WIDTH = temp_map_width;
    MAP_HEIGHT = temp_map_height;

    // allocate the map array
    map = (int **)malloc(MAP_HEIGHT * sizeof(int *));
    if (!map)
    {
        printf("Failed to allocate memory for map rows.\n");
        fclose(file);
        return -1;
    }
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        map[i] = (int *)malloc(MAP_WIDTH * sizeof(int));
        if (!map[i])
        {
            printf("Failed to allocate memory for map columns.\n");
            fclose(file);
            return -1;
        }
    }

    // reset file pointer for second pass
    fseek(file, 0, SEEK_SET);
    map_section = 0;
    current_row = 0;

    // second pass: load the actual map data
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if (strstr(line, "[MAP]") != NULL)
        {
            map_section = 1;
            continue;
        }
        else if (strstr(line, "[TEXTURES]") != NULL)
        {
            break;
        }

        if (map_section && strlen(line) > 0 && line[0] != '[')
        {
            if (current_row >= MAP_HEIGHT)
                break;

            char *token = strtok(line, " ");
            int col = 0;
            while (token != NULL && col < MAP_WIDTH)
            {
                map[current_row][col] = atoi(token);
                token = strtok(NULL, " ");
                col++;
            }
            if (col != MAP_WIDTH)
            {
                printf("Warning: Row %d has unexpected number of columns.\n", current_row);
            }
            current_row++;
        }
    }

    // read texture info (after the [Textures] section)
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, "skybox=") != NULL)
        {
            sscanf(line, "skybox=%255[^\n]", skybox);
            printf("Loaded texture: %s\n", skybox);
            break;
        }
        if (strstr(line, "1=") != NULL)
        {
            sscanf(line, "1=%s", wallsTexture);
            printf("Loaded texture: %s\n", wallsTexture);
            break;
        }
    }

    fclose(file);
    printf("Map loaded successfully!\n\n"); // yeah, the map loaded!
    if (strcmp(branch, "beta") == 0)
    {
        printf("╔════════════ WARNING ════════════╗\n");
        printf("║       Beta map detected!        ║\n");
        printf("║      These can be unstable!     ║\n");
        printf("╚═════════════════════════════════╝\n\n");
        printf("Map Info:\n");
        printf("├─ Version: %.1f\n", map_version);
        printf("├─ Map Path: %s\n", filename);
        printf("├─ Branch: %s\n", branch);
        printf("├─ Creator: %s\n", creator);
        printf("├─ Song: %s\n", songMap);
        printf("├─ Description: %s\n", description);
        printf("└─ Ambient Light: %.1f\n\n", ambient_light);
    }

    return 0;
}
