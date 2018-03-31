#include <nta/GLMConsoleOutput.h>
#include <nta/ScreenManager.h>
#include <nta/Logger.h>
#include <nta/MyEngine.h>

#include "MainGame.h"
#include "MainMenu.h"

#define log(s) nta::Logger::writeToLog(#s + std::string(": ") + nta::to_string(s))

void log_defs() {
    nta::Logger::writeToLog("Various Parameters:");
    nta::Logger::indent();

    log(TILE_SIZE);
    //log(PLAYER_DIMS);
    log(PLAYER_AREA);
    log(PLAYER_MASS);
    log(PLAYER_FORCE);
    log(PLAYER_JUMP_FORCE);
    //log(ENEMY_UNIT_DIMS);
    log(SMALL_ITEM_SIZE);

    nta::Logger::unindent();
}

int main(int argc, char* argv[]) {
    nta::init();
    log_defs();
    
    nta::ScreenManager screenManager(TITLE, TARGET_FPS);
    screenManager.addScreen(new MainMenu);
    screenManager.addScreen(new MainGame, MAINMENU_SCREEN_INDEX);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(); 
    
    screenManager.destroy();
    nta::cleanup();
    nta::Logger::writeToLog("Program exited cleanly");
    return 0;
}
