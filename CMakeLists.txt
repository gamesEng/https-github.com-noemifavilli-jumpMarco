cmake_minimum_required(VERSION 3.9)
# Compiler flags
if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc" CACHE INTERNAL "" FORCE)
elseif(APPLE)
  message("hello apple")
  set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9 CACHE STRING "")
endif()
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



## custom target?
#add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND} 
#  -E copy_directory
#    "${PROJECT_SOURCE_DIR}/res"
#    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/res
#)

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
target_include_directories(engine INTERFACE "${CMAKE_SOURCE_DIR}/lib/engine" PUBLIC SYSTEM ${B2D_INCS} )
target_link_libraries(engine tile_level_loader maths ecm Box2D sfml-graphics sfml-audio)

## Maths ####
add_library(maths INTERFACE)
target_sources(maths INTERFACE "${CMAKE_SOURCE_DIR}/lib/maths/maths.h")
target_include_directories(maths INTERFACE "${CMAKE_SOURCE_DIR}/lib/maths" INTERFACE ${SFML_INCS})

#### Tile Level Loader ####
file(GLOB_RECURSE SOURCE_FILES lib/tile_level_loader/*.cpp lib/tile_level_loader/*.h)
add_library(tile_level_loader STATIC ${SOURCE_FILES} ${RESOURCE_FILES})
target_include_directories(tile_level_loader INTERFACE "${CMAKE_SOURCE_DIR}/lib/tile_level_loader" )
target_link_libraries(tile_level_loader maths sfml-graphics)

#### Entity Component Model ####
file(GLOB_RECURSE SOURCE_FILES lib/ecm/*.cpp lib/ecm/*.h)
add_library(ecm STATIC ${SOURCE_FILES})
target_include_directories(ecm INTERFACE "${CMAKE_SOURCE_DIR}/lib/ecm" )
target_link_libraries(ecm PRIVATE maths)


###FILES
file(GLOB_RECURSE SOURCES jump_marco/*.cpp jump_marco/*.h)
# Components
file(GLOB_RECURSE CMPNTS jump_marco/components/*.cpp jump_marco/components/*.h)
file(GLOB_RECURSE ENTS jump_marco/entities/*.cpp jump_marco/entities/*.h)
file(GLOB_RECURSE SCENES jump_marco/scenes/*.cpp jump_marco/scenes/*.h)

##Gtest
find_package(GTest CONFIG REQUIRED)
add_executable(UnitTests 
    test/main_test.cpp
    test/entities/test_player_factory.cpp
    jump_marco/game.h
    jump_marco/main.cpp
    ${CMPNTS}
    ${ENTS}
    ${SCENES}
)
target_link_libraries(UnitTests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main 
    engine maths Box2D ecm
    optimized sfml-audio debug sfml-audio-d
    optimized sfml-window debug sfml-window-d
    optimized sfml-system debug sfml-system-d
    optimized sfml-graphics debug sfml-graphics-d
)
target_include_directories(UnitTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/jump_marco     # Include jump_marco folder
    ${CMAKE_SOURCE_DIR}/test           # Include test folder
)
enable_testing()
add_test(NAME UnitTests COMMAND UnitTests)

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

#### Jump Marco ####


add_executable(JUMP_MARCO ${SOURCES} ${RESOURCE_FILES})

source_group("components" FILES ${CMPNTS})
source_group("scenes" FILES ${SCENES})
source_group("entities" FILES ${ENTS})
source_group("resources" FILES ${RESOURCE_FILES})

#target_include_directories(JUMP_MARCO PUBLIC ${SFML_INCS})
##target_link_libraries(JUMP_MARCO PUBLIC engine)
set(EXECUTABLES ${EXECUTABLES} JUMP_MARCO)

### 
##add_dependencies(JUMP_MARCO copy_resources)

set_target_properties(JUMP_MARCO 
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
)



#### Target for copying resources to build dir####
if(APPLE)
  
  # Ensure `res` is bundled with Xcode builds
  if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties(JUMP_MARCO PROPERTIES
      XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"  # Adjust as necessary
    )
  endif()
  add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/res" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/lib/sfml/extlibs/libs-osx/Frameworks" ${CMAKE_BINARY_DIR}/lib/sfml/Frameworks
  )
else()
  add_custom_target(copy_resources ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/res" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/res
  )
endif()

#### Link Dependencies ####
foreach (exe ${EXECUTABLES})
  #set working directory to build dir
  set_target_properties(${exe} PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
  )

  target_link_libraries(${exe}
    engine maths Box2D ecm
    optimized sfml-audio debug sfml-audio-d
    optimized sfml-window debug sfml-window-d
    optimized sfml-system debug sfml-system-d
    optimized sfml-graphics debug sfml-graphics-d
    optimized sfml-main
  )
  
  if(MSVC) # Hide Console in release builds
    set_target_properties(${exe} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  endif(MSVC)
  
  add_dependencies(${exe} sfml-graphics sfml-audio copy_resources)
endforeach ()


#Install directory within build folder, so it doesn't go into /lib or program-files
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/")
#Installer.exe/zip built into /installer
SET(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/installer/")

#Install component for runtime dlls and stuff
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE) #stop this happening on non-release buildss
INCLUDE(InstallRequiredSystemLibraries)
install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "bin/" CONFIGURATIONS Release COMPONENT System )
#Install component for resources
install(DIRECTORY "${PROJECT_SOURCE_DIR}/res" DESTINATION "bin/" CONFIGURATIONS Release COMPONENT resources_cmpt)
#We disabled SFML's INSTALL component as it brings along junk, so we gotta make one ourselves
install(DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/" DESTINATION "bin" CONFIGURATIONS Release COMPONENT libs_cmpt FILES_MATCHING PATTERN "*.dll" )

#Tell CPack about the libs and resources install components
set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} resources_cmpt libs_cmpt)
#CPACK_INSTALL_CMAKE_PROJECTS IS only necessary if not using CPACK_COMPONENTS_ALL
#set(CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};resources_cmpt;/)
#set(CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};libs_cmpt;/)

#Install component for all labs, also tell CPack about them
foreach (exe ${EXECUTABLES})
  install(TARGETS ${exe} DESTINATION "bin" CONFIGURATIONS Release COMPONENT ${exe}_cmpt )
  #set(CPACK_INSTALL_CMAKE_PROJECTS ${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};${exe}_cmpt;/)
  set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} ${exe}_cmpt)
  set(CPACK_COMPONENT_${exe}_CMPT_DEPENDS resources_cmpt libs_cmpt)
  set(CPACK_COMPONENT_${exe}_CMPT_GROUP "Labs")
endforeach ()


IF(WIN32)
  set(CPACK_GENERATOR NSIS)
  set(CPACK_NSIS_COMPONENT_INSTALL ON)
  set(CPACK_NSIS_HELP_LINK "https://github.com/edinburgh-napier/set09121")
  set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/edinburgh-napier/set09121")
  set(CPACK_NSIS_CONTACT "https://github.com/edinburgh-napier")
ELSE()
  set(CPACK_GENERATOR ZIP)
ENDIF(WIN32)


set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "set09119")
set(CPACK_PACKAGE_VENDOR "Edinburgh Napier University")
find_file (LICENSE NAMES LICENSE LICENSE.txt PATHS  "${CMAKE_SOURCE_DIR}/../" "${CMAKE_SOURCE_DIR}" NO_DEFAULT_PATH)
if(${LICENSE})
  set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE})
endif(${LICENSE})
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")

INCLUDE(CPack)