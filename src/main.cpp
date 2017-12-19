#include <SDL2/SDL_ttf.h>

#include "nta/ResourceManager.h"
#include "nta/SystemManager.h"
#include "nta/AudioManager.h"
#include "nta/ScreenManager.h"
#include "nta/Logger.h"
#include "nta/MyEngine.h"
#include "nta/Random.h"

#include "MainGame.h"

int main(int argc, char* argv[]) {
    nta::init();
    nta::Logger::createLog();
    nta::Random::init();
    nta::AudioManager::init();
    
    nta::ScreenManager screenManager("Game", 60);
    screenManager.addScreen(new MainGame);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(); 
    
    screenManager.destroy();
    nta::ResourceManager::destroy();
    nta::SystemManager::destroy();
    nta::AudioManager::destroy();
    TTF_Quit();
    SDL_Quit();
    nta::Logger::writeToLog("Program exited cleanly");
    return 0;
}
