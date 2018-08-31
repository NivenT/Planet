cmake_minimum_required(VERSION 2.8.7)
project(game)

option(LINK_RESOURCES "Automatically creates links to resources/shaders" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Uncomment to use CImg instead of DevIL
#set(NTA_USE_DEVIL OFF CACHE BOOL "" FORCE)
set(NTA_USE_AUDIO OFF CACHE BOOL "" FORCE)
set(NTA_USE_IMGUI ON CACHE BOOL "" FORCE)
set(NTA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(NTA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(jubilant-funicular)

set(BOX2D_BUILD_EXAMPLES OFF CACHE BOOL "Build Box2D examples" FORCE)
add_subdirectory(Box2D)

add_library(lambertw lambertw/LambertW.c)

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
                    src/Spawner.cpp
                    src/World.cpp
                    src/PhysicsComponent.cpp
                    src/GraphicsComponent.cpp
                    src/ControllerComponent.cpp
                    src/HealthComponent.cpp)
# What is dl? (Needed for ChaiScript)
target_link_libraries(game ${NTA_LIBRARIES} Box2D dl lambertw stdc++fs)
target_include_directories(game PUBLIC include 
                                       ${NTA_INCLUDE_DIR} 
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
