#include <SDL2/SDL_ttf.h>

#include <nta/GLMConsoleOutput.h>
#include <nta/CallbackManager.h>
#include <nta/ResourceManager.h>
#include <nta/SystemManager.h>
#include <nta/AudioManager.h>
#include <nta/ScreenManager.h>
#include <nta/Logger.h>
#include <nta/MyEngine.h>
#include <nta/Random.h>

#include "MainGame.h"

#define log(s) nta::Logger::writeToLog(#s + std::string(": ") + nta::to_string(s))

void log_defs() {
    nta::Logger::writeToLog("Various Parameters:");
    nta::Logger::indent();

    log(TILE_SIZE);
    log(PLAYER_DIMS);
    log(PLAYER_AREA);
    log(PLAYER_MASS);
    log(PLAYER_FORCE);
    log(PLAYER_JUMP_FORCE);
    log(ENEMY_UNIT_DIMS);
    log(SMALL_ITEM_SIZE);

    nta::Logger::unindent();
}

int main(int argc, char* argv[]) {
    nta::init();
    nta::Logger::createLog();
    nta::Random::init();
    nta::AudioManager::init();
    nta::CallbackManager::init();

    log_defs();
    
    nta::ScreenManager screenManager(TITLE, TARGET_FPS);
    screenManager.addScreen(new MainGame);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(); 
    
    screenManager.destroy();
    nta::ResourceManager::destroy();
    nta::SystemManager::destroy();
    nta::AudioManager::destroy();
    nta::CallbackManager::destroy();
    TTF_Quit();
    SDL_Quit();
    nta::Logger::writeToLog("Program exited cleanly");
    return 0;
}
