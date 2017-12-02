#include <SDL2/SDL_ttf.h>

#include "nta/ResourceManager.h"
#include "nta/SystemManager.h"
#include "nta/AudioManager.h"
#include "nta/Logger.h"
#include "nta/MyEngine.h"
#include "nta/Random.h"

int main(int argc, char* argv[]) {
    nta::init();
    nta::Logger::createLog();
    nta::Random::init();
    nta::AudioManager::init();

    nta::ScreenManager screenManager("Chip-8", 60);
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
