#ifndef MAINGAME_H_INCLUDED
#define MAINGAME_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/DebugBatch.h>
#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Screen.h>

#include "World.h"

class MainGame : public nta::Screen {
private:
    void prepare_batches();
    void render_batches();
    void debug_update();
    void dev_update();
    glm::vec2 getMouse() const;

    nta::GLSLProgram* m_simpleProg = nullptr;
    nta::GLSLProgram* m_overlayProg = nullptr;
    nta::GLSLProgram* m_planetProg = nullptr;
    nta::SpriteFont* m_font = nullptr;

    // Never expected to ever use more than 2 batches
    nta::SpriteBatch m_batch;
    nta::SpriteBatch m_overlay_batch;
    nta::DebugBatch m_debug_batch;
    nta::Camera2D m_camera;

    World* m_world = nullptr;

    // This is getting ridiculous
    bool m_debug;
    bool m_soft_debug;
    bool m_draw_aabbs;
    bool m_square_planet;
    bool m_paused;
    bool m_dev_mode;
public:
    MainGame();
    ~MainGame();
    void onFocus(const nta::ScreenSwitchInfo& info);
    void offFocus();
    void update();
    void render();
    void init();
};

#endif // MAINGAME_H_INCLUDED
