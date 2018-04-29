cmake_minimum_required(VERSION 2.8.7)
project(game)

if(NOT ${CMAKE_VERSION} LESS 3.2)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    message(STATUS "Using C++14")
else()
    message(STATUS "Checking compiler flags for C++14 support.")
    # Set C++11/14 support flags for various compilers
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX14)
        message(STATUS "C++14 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
        endif()
    elseif(COMPILER_SUPPORTS_CXX11)
        message(STATUS "C++11 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUS "C++0x is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        endif()
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} does not have recent enough C++ support. Please use a different C++ compiler.")
    endif()
endif()

option(LINK_RESOURCES "Automatically creates links to resources/shaders" ON)

# Uncomment to use CImg instead of DevIL
#set(NTA_USE_DEVIL OFF CACHE BOOL "" FORCE)
set(NTA_USE_AUDIO OFF CACHE BOOL "" FORCE)
add_subdirectory(jubilant-funicular)

set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "Build Box2D examples" FORCE)
add_subdirectory(Box2D)

add_library(lambertw lambertw/LambertW.c)

find_package(SDL2 REQUIRED)
add_library(imgui imgui/imgui.cpp
                  imgui/imgui_draw.cpp
                  imgui/imgui_impl_sdl_gl3.cpp)

add_executable(game src/main.cpp 
                    src/MainGame.cpp 
                    src/Planet.cpp 
                    src/Object.cpp
                    src/Agent.cpp
                    src/Player.cpp
                    src/utils.cpp
                    src/Item.cpp
                    src/Enemy.cpp
                    src/MainMenu.cpp
                    src/ChaiManager.cpp
                    src/WorldEditor.cpp
                    src/Spawner.cpp)
# What is dl? (Needed for ChaiScript)
target_link_libraries(game ${NTA_LIBRARIES} Box2D dl lambertw imgui)
include_directories(include ${NTA_INCLUDE_DIR} 
                            ${CMAKE_CURRENT_SOURCE_DIR}/Box2D
                            ${CMAKE_CURRENT_SOURCE_DIR}/chaiscript)

if(${LINK_RESOURCES})
	if (NOT EXISTS "${CMAKE_BINARY_DIR}/resources")
		message(STATUS "Linking resources, shaders, and scripts")
		message(STATUS "If this fails (e.g. if you're on Windows), turn off the LINK_RESOURCES flag in CMakeLists.txt")
		execute_process(COMMAND bash -c "ln -s ${CMAKE_CURRENT_SOURCE_DIR}/resources")
		execute_process(COMMAND bash -c "ln -s ${CMAKE_CURRENT_SOURCE_DIR}/shaders")
        execute_process(COMMAND bash -c "ln -s ${CMAKE_CURRENT_SOURCE_DIR}/scripts")
	endif()
endif()
