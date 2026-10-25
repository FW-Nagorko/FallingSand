cmake_minimum_required(VERSION 3.11)
project(FallingSand LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use SDL3 and SDL3_ttf supplied by the system or development environment.
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

# Set Properties
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define source files
set(SOURCES
    src/game.c
    src/main.c
    src/rendering.c
    src/ui.c
    vendor/Vector/Vector.c
)

# Define header files (for IDE support)
set(HEADERS
    src/game.h
    src/rendering.h
    src/ui.h
    vendor/clay/clay.h
    vendor/Vector/Vector.h
)

# Create the executable
add_executable(FallingSand ${SOURCES} ${HEADERS})

# Link to the SDL3 libraries found above
target_link_libraries(FallingSand PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

# Set target properties
set_target_properties(FallingSand PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# For Windows, copy SDL3.dll to the output directory
if(WIN32)
    add_custom_command(
        TARGET FallingSand POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
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
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(FallingSand PRIVATE -g -O0)
    endif()
    target_compile_definitions(FallingSand PRIVATE DEBUG=1)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# Print build information
message(STATUS "FallingSand build configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Using installed SDL3 and SDL3_ttf packages")
