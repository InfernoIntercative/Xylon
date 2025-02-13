cmake_minimum_required(VERSION 3.10)
project(XylonEngine)

# Set policies
cmake_policy(SET CMP0072 NEW)

# Source, object, and binary directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OBJ_DIR ${CMAKE_BINARY_DIR}/obj)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# Executable name
set(EXEC XylonEngine)

# Define source files
set(SOURCES
    ${SRC_DIR}/Core/main.cpp
    ${SRC_DIR}/Graphics/g_graphics.cpp
    ${SRC_DIR}/Player/p_player.cpp
    ${SRC_DIR}/Levels/l_levels.cpp
    ${SRC_DIR}/Texts/t_text.cpp
    ${SRC_DIR}/Console/c_console.cpp
)

# Find external packages
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include directories
include_directories(
    ${SRC_DIR}/Core
    ${SRC_DIR}/Levels
    ${SRC_DIR}/Player
    ${SRC_DIR}/Graphics
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Libraries to link
set(LIBS
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)

# Create executable
add_executable(${EXEC} ${SOURCES})

# Link libraries
target_link_libraries(${EXEC} ${LIBS})

# Set output directories
set_target_properties(${EXEC} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Visual Studio specific settings (if needed)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXEC})

# Add Debug/Release build settings
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Clean build directories efficiently
add_custom_target(clean_project
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${OBJ_DIR} ${BIN_DIR}
    COMMENT "Cleaning build directories"
)
