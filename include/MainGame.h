#ifndef MAINGAME_H_INCLUDED
#define MAINGAME_H_INCLUDED

#include <memory>

#include <Box2D/Box2D.h>

#include <nta/PrimitiveBatch.h>
#include <nta/SpriteBatch.h>
#include <nta/SpriteFont.h>
#include <nta/GLSLProgram.h>
#include <nta/Camera2D.h>
#include <nta/Screen.h>

#include "Planet.h"

#define DEBUG_BOX2D_COLOR glm::vec4(0.5,0,0.3,1)
#define DEBUG_BOX2D_AABB_COLOR glm::vec4(1,0,0,1)

class MainGame : public nta::Screen {
private:
    // TODO (?): Move to a util.{h,cpp} file?
    void debug_render_aabb(nta::PrimitiveBatch& pbatch, const b2AABB& box) const;
    void debug_render_poly(nta::PrimitiveBatch& pbatch, const b2PolygonShape* poly,
                           glm::vec2 position, float rotation) const;
    void debug_render_body(nta::PrimitiveBatch& pbatch, const b2Body* body, 
                           bool draw_aabbs) const;
    void debug_render_world(nta::PrimitiveBatch& pbatch, const b2World* world, 
                            bool draw_aabbs) const;

    nta::GLSLProgram* m_simpleProg = nullptr;
    nta::GLSLProgram* m_overlayProg = nullptr;
    nta::GLSLProgram* m_planetProg = nullptr;
    nta::SpriteFont* m_font = nullptr;

    nta::SpriteBatch m_batch;
    nta::SpriteBatch m_overlay_batch;
    nta::PrimitiveBatch m_pbatch;
    nta::Camera2D m_camera;

    std::unique_ptr<b2World> m_world;
    Planet m_planet;
    float m_time;
    bool m_debug;
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
