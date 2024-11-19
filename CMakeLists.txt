cmake_minimum_required(VERSION 3.11)

# Set toolchain to use Vcpkg to get libs
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/lib/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
message(STATUS "Using Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
# Require modern C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Games_Engineering)

#### Setup Directories ####
#Main output directory
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
# Ouput all DLLs from all libs into main build folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

#### Add External Dependencies ####
add_subdirectory("lib/sfml")
set(SFML_INCS "lib/sfml/include")
link_directories("${CMAKE_BINARY_DIR}/lib/sfml/lib")


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
file(GLOB_RECURSE SOURCES jump_marco/*.cpp jump_marco/*.h)
add_executable(JUMP_MARCO ${SOURCES})
target_include_directories(JUMP_MARCO PRIVATE ${SFML_INCS})
target_link_libraries(JUMP_MARCO sfml-graphics)