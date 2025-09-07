cmake_minimum_required(VERSION 3.5)
project(FallingSand LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Create an option to switch between a system SDL library and a vendored SDL library
option(FALLINGSAND_VENDORED "Use vendored SDL library" OFF)

if(FALLINGSAND_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # Look for a SDL3 package, look for the SDL3-shared component, and fail if it cannot be found
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# Define source files
set(SOURCES
    src/main.c
    src/game.c
    src/rendering.c
)

# Define header files (for IDE support)
set(HEADERS
    src/game.h
    src/rendering.h
)

# Create the executable
add_executable(FallingSand ${SOURCES} ${HEADERS})

# Link to the SDL3 library
target_link_libraries(FallingSand PRIVATE SDL3::SDL3)

# Set target properties
set_target_properties(FallingSand PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# For Windows, copy SDL3.dll to the output directory
if(WIN32 AND TARGET SDL3::SDL3-shared)
    add_custom_command(
        TARGET FallingSand POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:FallingSand>
        COMMENT "Copying SDL3.dll to output directory"
        VERBATIM
    )
endif()

# Add include directories for headers
target_include_directories(FallingSand PRIVATE src)

# Set compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(FallingSand PRIVATE -Wall -Wextra -Wpedantic)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_compile_options(FallingSand PRIVATE /W4)
endif()

# Set debug/release configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Debug-specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(FallingSand PRIVATE -g -O0)
    target_compile_definitions(FallingSand PRIVATE DEBUG=1)
endif()

# Print build information
message(STATUS "FallingSand build configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Using vendored SDL: ${FALLINGSAND_VENDORED}")
