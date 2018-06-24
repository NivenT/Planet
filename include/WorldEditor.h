#ifndef WORLDEDITOR_H_INCLUDED
#define WORLDEDITOR_H_INCLUDED

#include <nta/Screen.h>
#include <nta/Camera2D.h>
#include <nta/GLSLProgram.h>
#include <nta/SpriteFont.h>

#include "Spawner.h"
#include "Item.h"

class WorldEditor : public nta::Screen {
private:
    glm::vec2 screen_to_game(crvec2 screen) const;
    void prepare_batches();
    void render_batches(const nta::Camera2D camera);
    void render_miniworld();
    void render_gui();

    nta::SpriteFont* m_font = nullptr;
    nta::GLSLProgram* m_prog = nullptr;
    nta::GLSLProgram* m_overlay_prog = nullptr;
    nta::GLSLProgram* m_planet_prog = nullptr;
    nta::SpriteBatch m_batch;
    nta::SpriteBatch m_overlay_batch;
    nta::Camera2D m_camera;

    Planet m_planet;
    Tile m_active_tile;
    ItemParams m_active_item;

    int m_curr_tab = GUI_GENERAL_TAB;
    bool m_gui_focus = false;
    bool m_square_planet = false;
public:
    WorldEditor();
    ~WorldEditor();
    void onFocus();
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // WORLDEDITOR_H_INCLUDED
