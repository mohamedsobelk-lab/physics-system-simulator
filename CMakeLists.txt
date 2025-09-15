cmake_minimum_required(VERSION 3.20)
project(PhysicsSystemSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths for Box2D and SFML (adjust these paths as needed)
set(BOX2D_ROOT "path/to/box2d-3.1.0" CACHE PATH "Path to Box2D installation")
set(SFML_ROOT "path/to/sfml-3.0.1" CACHE PATH "Path to SFML installation")

# Find packages using config mode
find_package(Box2D 3.1.0 REQUIRED CONFIG HINTS ${BOX2D_ROOT})
find_package(SFML 3.0.1 REQUIRED COMPONENTS graphics window system CONFIG HINTS ${SFML_ROOT})

# Add executable
add_executable(PhysicsSimulator
    Main.cpp
    PhysicsDebugDraw.h
)

# Link libraries
target_link_libraries(PhysicsSimulator
    Box2D::Box2D
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Include directories
target_include_directories(PhysicsSimulator PRIVATE
    ${BOX2D_INCLUDE_DIRS}
    ${SFML_INCLUDE_DIRS}
)

# Copy DLLs for Windows (if using shared libraries)
if(WIN32 AND NOT SFML_STATIC_LIBRARIES)
    add_custom_command(TARGET PhysicsSimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:SFML::Graphics>
        $<TARGET_FILE_DIR:PhysicsSimulator>
    )
    add_custom_command(TARGET PhysicsSimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:SFML::Window>
        $<TARGET_FILE_DIR:PhysicsSimulator>
    )
    add_custom_command(TARGET PhysicsSimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:SFML::System>
        $<TARGET_FILE_DIR:PhysicsSimulator>
    )
    # Box2D typically doesn't need DLL copying as it's usually built as static lib
endif()

# Set C++17 standard (as requested)
set_target_properties(PhysicsSimulator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)