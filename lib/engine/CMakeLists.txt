add_library(engine STATIC
    src/engine.cpp
    src/system_physics.cpp
    src/system_renderer.cpp
    src/system_resources.cpp
)

target_include_directories(engine PUBLIC include
)