cmake_minimum_required(VERSION 3.11)

# Set toolchain to use Vcpkg to get libs
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/lib/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
message(STATUS "Using Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Games_Engineering)

#### Setup Directories ####
#Main output directory
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Ouput all DLLs from all libs into main build folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})



## custom target?
add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} 
  -E copy_directory
    "${PROJECT_SOURCE_DIR}/res"
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/res
)

#### Add External Dependencies ####
add_subdirectory("lib/sfml" EXCLUDE_FROM_ALL)
set(SFML_INCS "lib/sfml/include")
link_directories("${CMAKE_BINARY_DIR}/lib/sfml/lib")

# B2D - Box2D phyics library
add_subdirectory("lib/b2d/Box2D")
#include_directories("lib/b2d/Box2D/")
set(B2D_INCS "lib/b2d/Box2D/")
link_directories("${CMAKE_BINARY_DIR}/lib/sfml/lib")


### Own Libraries ###
file(GLOB_RECURSE RESOURCE_FILES "res/*.*")
#### Engine ####
file(GLOB_RECURSE SOURCE_FILES lib/engine/*.cpp lib/engine/*.h)
add_library(engine STATIC ${SOURCE_FILES} ${RESOURCE_FILES})
target_include_directories(engine INTERFACE "${CMAKE_SOURCE_DIR}/lib/engine/" PUBLIC SYSTEM ${B2D_INCS} )
target_link_libraries(engine tile_level_loader maths ecm Box2D sfml-graphics sfml-audio)

## Maths ####
add_library(maths INTERFACE)
target_sources(maths INTERFACE "${CMAKE_SOURCE_DIR}/lib/maths/maths.h")
target_include_directories(maths INTERFACE "${CMAKE_SOURCE_DIR}/lib/maths" INTERFACE ${SFML_INCS})

#### Tile Level Loader ####
file(GLOB_RECURSE SOURCE_FILES lib/tile_level_loader/*.cpp lib/tile_level_loader/*.h)
add_library(tile_level_loader STATIC ${SOURCE_FILES} ${RESOURCE_FILES})
target_include_directories(tile_level_loader INTERFACE "${CMAKE_SOURCE_DIR}/lib/tile_level_loader/" )
target_link_libraries(tile_level_loader maths sfml-graphics)

#### Entity Component Model ####
file(GLOB_RECURSE SOURCE_FILES lib/ecm/*.cpp lib/ecm/*.h)
add_library(ecm STATIC ${SOURCE_FILES})
target_include_directories(ecm INTERFACE "${CMAKE_SOURCE_DIR}/lib/ecm" )
target_link_libraries(ecm PRIVATE maths)


# Opening Vcpkg win OR linux
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/lib/vcpkg/vcpkg")
    message(STATUS "Bootstrapping vcpkg...")

    if(WIN32)
        execute_process(
            COMMAND cmd /c bootstrap-vcpkg.bat
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/vcpkg"
            RESULT_VARIABLE result
        )
    elseif(UNIX)
        execute_process(
            COMMAND ./bootstrap-vcpkg.sh
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/vcpkg"
            RESULT_VARIABLE result
        )
    else()
        message(FATAL_ERROR "Unsupported platform for vcpkg bootstrap")
    endif()

    if(result)
        message(FATAL_ERROR "vcpkg bootstrap failed with result: ${result}")
    endif()

endif()

find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(UnitTests test/main_test.cpp)

# Use the vcpkg-installed GTest libraries
target_link_libraries(UnitTests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME UnitTests COMMAND UnitTests)


#### Jump Marco ####
###FILES
file(GLOB_RECURSE SOURCES jump_marco/*.cpp jump_marco/*.h)
# Components
file(GLOB_RECURSE CMPNTS jump_marco/components/*.cpp jump_marco/components/*.h)
file(GLOB_RECURSE SCENES jump_marco/scenes/*.cpp jump_marco/scenes/*.h)

add_executable(JUMP_MARCO ${SOURCES} ${RESOURCE_FILES})

source_group("components" FILES ${CMPNTS})
source_group("resources" FILES ${RESOURCE_FILES})

#target_include_directories(JUMP_MARCO PUBLIC ${SFML_INCS})
target_link_libraries(JUMP_MARCO engine)
set(EXECUTABLES ${EXECUTABLES} JUMP_MARCO)

### 
add_dependencies(JUMP_MARCO copy_resources)

set_target_properties(JUMP_MARCO 
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
)