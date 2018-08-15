#ifndef WORLDEDITOR_H_INCLUDED
#define WORLDEDITOR_H_INCLUDED

#include <nta/Screen.h>
#include <nta/Camera2D.h>
#include <nta/GLSLProgram.h>
#include <nta/SpriteFont.h>
#include <nta/DebugBatch.h>

#include "World.h"

// This is gonna end up having an obscene amount of functions...
class WorldEditor : public nta::Screen {
private:
    glm::vec2 screen_to_game(crvec2 screen) const;
    bool gui_in_use() const;

    void update_camera();
    void update_tile_tab(crvec2 mouse, const glm::ivec2& coord);
    void update_item_tab(crvec2 mouse);
    void update_enemy_tab(crvec2 mouse);
    void update_spawner_tab(crvec2 mouse);

    void prepare_batches();
    void render_batches(const nta::Camera2D camera, bool debug);
    void render_miniworld();
    void render_gui();
    
    void render_planet_tab();
    void render_tile_tab();
    void render_item_tab();
    void render_enemy_tab();
    void render_spawner_tab();
    void render_obstacle_tab();

    nta::SpriteFont* m_font = nullptr;
    nta::GLSLProgram* m_prog = nullptr;
    nta::GLSLProgram* m_overlay_prog = nullptr;
    nta::GLSLProgram* m_planet_prog = nullptr;
    nta::SpriteBatch m_batch;
    nta::SpriteBatch m_overlay_batch;
    nta::DebugBatch m_dbatch;
    nta::Camera2D m_camera;

    WorldParams m_world;

    Tile m_active_tile;
    ItemParams m_active_item;
    EnemyParams m_active_enemy;
    SpawnerParams m_active_spawner;

    int m_curr_tab = GUI_PLANET_TAB;
    bool m_gui_focus = true;
    bool m_gui_active = true;
    bool m_square_planet = false;
public:
    WorldEditor();
    ~WorldEditor();
    WorldParams* get_world();
    void onFocus(const nta::ScreenSwitchInfo& info);
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // WORLDEDITOR_H_INCLUDED
