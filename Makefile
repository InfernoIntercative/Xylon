# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/russian95/Zypher

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/russian95/Zypher

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/russian95/Zypher/CMakeFiles /home/russian95/Zypher//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/russian95/Zypher/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named XylonEngine

# Build rule for target.
XylonEngine: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 XylonEngine
.PHONY : XylonEngine

# fast build rule for target.
XylonEngine/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/build
.PHONY : XylonEngine/fast

#=============================================================================
# Target rules for targets named clean_project

# Build rule for target.
clean_project: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean_project
.PHONY : clean_project

# fast build rule for target.
clean_project/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/clean_project.dir/build.make CMakeFiles/clean_project.dir/build
.PHONY : clean_project/fast

src/Core/main.o: src/Core/main.cpp.o
.PHONY : src/Core/main.o

# target to build an object file
src/Core/main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Core/main.cpp.o
.PHONY : src/Core/main.cpp.o

src/Core/main.i: src/Core/main.cpp.i
.PHONY : src/Core/main.i

# target to preprocess a source file
src/Core/main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Core/main.cpp.i
.PHONY : src/Core/main.cpp.i

src/Core/main.s: src/Core/main.cpp.s
.PHONY : src/Core/main.s

# target to generate assembly for a file
src/Core/main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Core/main.cpp.s
.PHONY : src/Core/main.cpp.s

src/Graphics/g_graphics.o: src/Graphics/g_graphics.cpp.o
.PHONY : src/Graphics/g_graphics.o

# target to build an object file
src/Graphics/g_graphics.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Graphics/g_graphics.cpp.o
.PHONY : src/Graphics/g_graphics.cpp.o

src/Graphics/g_graphics.i: src/Graphics/g_graphics.cpp.i
.PHONY : src/Graphics/g_graphics.i

# target to preprocess a source file
src/Graphics/g_graphics.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Graphics/g_graphics.cpp.i
.PHONY : src/Graphics/g_graphics.cpp.i

src/Graphics/g_graphics.s: src/Graphics/g_graphics.cpp.s
.PHONY : src/Graphics/g_graphics.s

# target to generate assembly for a file
src/Graphics/g_graphics.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Graphics/g_graphics.cpp.s
.PHONY : src/Graphics/g_graphics.cpp.s

src/Map/m_map.o: src/Map/m_map.cpp.o
.PHONY : src/Map/m_map.o

# target to build an object file
src/Map/m_map.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Map/m_map.cpp.o
.PHONY : src/Map/m_map.cpp.o

src/Map/m_map.i: src/Map/m_map.cpp.i
.PHONY : src/Map/m_map.i

# target to preprocess a source file
src/Map/m_map.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Map/m_map.cpp.i
.PHONY : src/Map/m_map.cpp.i

src/Map/m_map.s: src/Map/m_map.cpp.s
.PHONY : src/Map/m_map.s

# target to generate assembly for a file
src/Map/m_map.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Map/m_map.cpp.s
.PHONY : src/Map/m_map.cpp.s

src/Player/p_player.o: src/Player/p_player.cpp.o
.PHONY : src/Player/p_player.o

# target to build an object file
src/Player/p_player.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Player/p_player.cpp.o
.PHONY : src/Player/p_player.cpp.o

src/Player/p_player.i: src/Player/p_player.cpp.i
.PHONY : src/Player/p_player.i

# target to preprocess a source file
src/Player/p_player.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Player/p_player.cpp.i
.PHONY : src/Player/p_player.cpp.i

src/Player/p_player.s: src/Player/p_player.cpp.s
.PHONY : src/Player/p_player.s

# target to generate assembly for a file
src/Player/p_player.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/XylonEngine.dir/build.make CMakeFiles/XylonEngine.dir/src/Player/p_player.cpp.s
.PHONY : src/Player/p_player.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... clean_project"
	@echo "... XylonEngine"
	@echo "... src/Core/main.o"
	@echo "... src/Core/main.i"
	@echo "... src/Core/main.s"
	@echo "... src/Graphics/g_graphics.o"
	@echo "... src/Graphics/g_graphics.i"
	@echo "... src/Graphics/g_graphics.s"
	@echo "... src/Map/m_map.o"
	@echo "... src/Map/m_map.i"
	@echo "... src/Map/m_map.s"
	@echo "... src/Player/p_player.o"
	@echo "... src/Player/p_player.i"
	@echo "... src/Player/p_player.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

