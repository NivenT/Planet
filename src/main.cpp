#include <nta/ScreenManager.h>
#include <nta/Logger.h>
#include <nta/MyEngine.h>

#include "MainGame.h"
#include "MainMenu.h"
#include "WorldEditor.h"
#include "ChaiManager.h"

#define log(s) nta::Logger::writeToLog(#s + std::string(": ") + nta::utils::to_string(s))

void log_defs() {
    nta::Logger::writeToLog("Various Parameters:");
    nta::Logger::indent();

    log(TILE_SIZE);
    //log(PLAYER_DIMS);
    log(PLAYER_INIT_HEALTH);
    log(PLAYER_AREA);
    log(PLAYER_MASS);
    log(PLAYER_FORCE);
    log(PLAYER_JUMP_FORCE);
    log(PLAYER_TILT_FORCE);
    log(PLAYER_WEAK_ATT_FORCE);
    //log(ENEMY_UNIT_DIMS);
    log(ENEMY_UNIT_AREA);
    log(NORMAL_ENEMY_INIT_HEALTH);
    log(SMALL_ITEM_SIZE);
    log(ENEMY_MIN_MAX_SPEED);
    log(ENEMY_MAX_MAX_SPEED);
    log(ENEMY_MIN_EXTENTS);
    log(ENEMY_MAX_EXTENTS);

    nta::Logger::unindent();
}

int main(int argc, char* argv[]) {
    nta::init();
    log_defs();
    ChaiManager::init();

    WorldEditor* world_editor_screen = new WorldEditor;

    nta::ScreenManager screenManager(TITLE, TARGET_FPS, 1000, 600);
    screenManager.addScreen(new MainMenu);
    screenManager.addScreen(new MainGame, MAINMENU_SCREEN_INDEX);
    screenManager.addScreen(world_editor_screen, MAINMENU_SCREEN_INDEX);

    nta::ErrorManager::set_periodic_callback([&](const nta::Error* errs) {
        while (errs) {
            if (errs->type != nta::DEVIL_FAILURE && errs->type != nta::MISSING_RESOURCE) {
                std::cout<<"An error occured. Check Log.log for details"<<std::endl;
                screenManager.switchScreen(-1);
            }
            errs = errs->prev;
        }
    });
    nta::ErrorManager::set_push_callback([](const nta::Error* _) {});

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    screenManager.run(world_editor_screen->get_world()); 
    
    screenManager.destroy();
    ChaiManager::destroy();
    nta::cleanup();
    nta::Logger::writeToLog("Program exited cleanly");
    return 0;
}
