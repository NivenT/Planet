#ifndef MAINGAME_H_INCLUDED
#define MAINGAME_H_INCLUDED

#include <memory>

#include <Box2D/Box2D.h>

#include <nta/DebugBatch.h>
#include <nta/SpriteBatch.h>
#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Screen.h>

#include "Planet.h"
#include "Player.h"
#include "utils.h"

#define DEFAULT_CAMERA_CENTER glm::vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y)
#define DEFAULT_CAMERA_DIMENSIONS glm::vec2(TILE_SIZE*3.f)

class MainGame : public nta::Screen {
private:
    void prepare_batches();
    void render_batches();

    nta::GLSLProgram* m_simpleProg = nullptr;
    nta::GLSLProgram* m_overlayProg = nullptr;
    nta::GLSLProgram* m_planetProg = nullptr;
    nta::SpriteFont* m_font = nullptr;

    // Never expected to ever use more than 2 batches
    nta::SpriteBatch m_batch;
    nta::SpriteBatch m_overlay_batch;
    nta::SpriteBatch m_light_batch;
    nta::SpriteBatch m_debug_sprite_batch;
    nta::DebugBatch m_debug_batch;
    nta::Camera2D m_camera;

    std::unique_ptr<b2World> m_world;
    std::vector<Object*> m_objects;
    Player* m_player;
    Planet m_planet;

    // This is getting ridiculous
    float m_time; // When am I gonna get rid of this?
    bool m_debug;
    bool m_soft_debug;
    bool m_draw_aabbs;
    bool m_square_planet;
    bool m_paused;
public:
    MainGame();
    ~MainGame();
    void update();
    void render();
    void init();
};

#endif // MAINGAME_H_INCLUDED
