# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /opt/clion-2021.1.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2021.1.1/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/erickelizondo/CLionProjects/Black-white-FileSystem

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Black_white_FileSystem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Black_white_FileSystem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Black_white_FileSystem.dir/flags.make

CMakeFiles/Black_white_FileSystem.dir/main.c.o: CMakeFiles/Black_white_FileSystem.dir/flags.make
CMakeFiles/Black_white_FileSystem.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Black_white_FileSystem.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Black_white_FileSystem.dir/main.c.o -c /home/erickelizondo/CLionProjects/Black-white-FileSystem/main.c

CMakeFiles/Black_white_FileSystem.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Black_white_FileSystem.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/erickelizondo/CLionProjects/Black-white-FileSystem/main.c > CMakeFiles/Black_white_FileSystem.dir/main.c.i

CMakeFiles/Black_white_FileSystem.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Black_white_FileSystem.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/erickelizondo/CLionProjects/Black-white-FileSystem/main.c -o CMakeFiles/Black_white_FileSystem.dir/main.c.s

CMakeFiles/Black_white_FileSystem.dir/fs.c.o: CMakeFiles/Black_white_FileSystem.dir/flags.make
CMakeFiles/Black_white_FileSystem.dir/fs.c.o: ../fs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/Black_white_FileSystem.dir/fs.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Black_white_FileSystem.dir/fs.c.o -c /home/erickelizondo/CLionProjects/Black-white-FileSystem/fs.c

CMakeFiles/Black_white_FileSystem.dir/fs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Black_white_FileSystem.dir/fs.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/erickelizondo/CLionProjects/Black-white-FileSystem/fs.c > CMakeFiles/Black_white_FileSystem.dir/fs.c.i

CMakeFiles/Black_white_FileSystem.dir/fs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Black_white_FileSystem.dir/fs.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/erickelizondo/CLionProjects/Black-white-FileSystem/fs.c -o CMakeFiles/Black_white_FileSystem.dir/fs.c.s

CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o: CMakeFiles/Black_white_FileSystem.dir/flags.make
CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o: ../structure_functions.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o -c /home/erickelizondo/CLionProjects/Black-white-FileSystem/structure_functions.c

CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/erickelizondo/CLionProjects/Black-white-FileSystem/structure_functions.c > CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.i

CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/erickelizondo/CLionProjects/Black-white-FileSystem/structure_functions.c -o CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.s

# Object files for target Black_white_FileSystem
Black_white_FileSystem_OBJECTS = \
"CMakeFiles/Black_white_FileSystem.dir/main.c.o" \
"CMakeFiles/Black_white_FileSystem.dir/fs.c.o" \
"CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o"

# External object files for target Black_white_FileSystem
Black_white_FileSystem_EXTERNAL_OBJECTS =

Black_white_FileSystem: CMakeFiles/Black_white_FileSystem.dir/main.c.o
Black_white_FileSystem: CMakeFiles/Black_white_FileSystem.dir/fs.c.o
Black_white_FileSystem: CMakeFiles/Black_white_FileSystem.dir/structure_functions.c.o
Black_white_FileSystem: CMakeFiles/Black_white_FileSystem.dir/build.make
Black_white_FileSystem: CMakeFiles/Black_white_FileSystem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable Black_white_FileSystem"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Black_white_FileSystem.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Black_white_FileSystem.dir/build: Black_white_FileSystem

.PHONY : CMakeFiles/Black_white_FileSystem.dir/build

CMakeFiles/Black_white_FileSystem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Black_white_FileSystem.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Black_white_FileSystem.dir/clean

CMakeFiles/Black_white_FileSystem.dir/depend:
	cd /home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erickelizondo/CLionProjects/Black-white-FileSystem /home/erickelizondo/CLionProjects/Black-white-FileSystem /home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug /home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug /home/erickelizondo/CLionProjects/Black-white-FileSystem/cmake-build-debug/CMakeFiles/Black_white_FileSystem.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Black_white_FileSystem.dir/depend

