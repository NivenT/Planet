#include <iostream>

#include <nta/Logger.h>
#include <nta/SystemManager.h>
#include <nta/ResourceManager.h>
#include <nta/InputManager.h>
#include <nta/ScreenManager.h>
#include <nta/Vertex.h>
#include <nta/Random.h>
#include <nta/GLMConsoleOutput.h>

#include "MainGame.h"
#include "utils.h"

using namespace std;
using namespace nta;
using namespace glm;

MainGame::MainGame() : m_time(0.), m_debug(true), m_square_planet(true), m_paused(true) {
    m_planet = Planet::new_test();
    m_world = make_unique<b2World>(m_planet.getGravity());

    m_planet.add_to_world(m_world.get());

    // Boxes for testing
    for (int i = 0; i < 20; i++) {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = b2Vec2(Random::randFloat(-80, 80), Random::randFloat(10, 100));
        b2Body* body = m_world->CreateBody(&bodyDef);

        b2PolygonShape boxShape;
        boxShape.SetAsBox(Random::randFloat(3, 8), Random::randFloat(3, 8));

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.3f;
        body->CreateFixture(&fixtureDef);
    }   
}

MainGame::~MainGame() {
}

void MainGame::debug_render_aabb(PrimitiveBatch& pbatch, const b2AABB& box) const {
    static const int NUM_PIECES = 150;

    b2Vec2 b2_center = box.GetCenter(), b2_extents = box.GetExtents();
    vec2 center(b2_center.x, b2_center.y), extents(b2_extents.x, b2_extents.y);

    for (int x = -1; x <= 1; x += 2) {
        vec2 start = center + vec2(x*extents.x, extents.y),
             end = center + vec2(x*extents.x, -extents.y);
        render_line_in_pieces(pbatch, start, end, NUM_PIECES, DEBUG_BOX2D_COLOR);
    }
    for (int y = -1; y <= 1; y += 2) {
        vec2 start = center + vec2(-extents.x, y*extents.y),
             end = center + vec2(extents.x, y*extents.y);
        render_line_in_pieces(pbatch, start, end, NUM_PIECES, DEBUG_BOX2D_COLOR);
    }
}

void MainGame::debug_render_body(PrimitiveBatch& pbatch, const b2Body* body) const {
    if (!body) return;
    // Not sure how I feel about this const placement
    b2Fixture const* curr = body->GetFixtureList();
    while (curr) {
        for (int i = 0; i < curr->GetShape()->GetChildCount(); i++) {
            debug_render_aabb(pbatch, curr->GetAABB(i));
        }
        curr = curr->GetNext();
    }
}

void MainGame::debug_render_world(PrimitiveBatch& pbatch, const b2World* world) const {
    b2Body const* curr = world->GetBodyList();
    while (curr) {
        debug_render_body(pbatch, curr);
        curr = curr->GetNext();
    }
}

void MainGame::init() {
    Logger::writeToLog("Initializing main screen...");

    m_font = ResourceManager::getSpriteFont("manilasansreg.otf");

    m_simpleProg = SystemManager::getGLSLProgram("simple2D");
    if (!m_simpleProg->isLinked()) {
        m_simpleProg->addAttribute("pos");
        m_simpleProg->addAttribute("color");
        m_simpleProg->addAttribute("uv");
        m_simpleProg->addAttribute("hasTexture");
        m_simpleProg->linkShaders();
    }
    m_simpleProg->use();
    glUniform1i(m_simpleProg->getUniformLocation("sampler"), 0);
    m_simpleProg->unuse();

    m_overlayProg = SystemManager::getGLSLProgram("overlay");
    if (!m_overlayProg->isLinked()) {
        m_overlayProg->addAttribute("pos");
        m_overlayProg->addAttribute("color");
        m_overlayProg->addAttribute("uv");
        m_overlayProg->addAttribute("hasTexture");
        m_overlayProg->linkShaders();
    }
    m_overlayProg->use();
    // TODO: remember what this does
    glUniform1i(m_overlayProg->getUniformLocation("sampler"), 0);
    m_overlayProg->unuse();

    m_planetProg = SystemManager::getGLSLProgram("planet");
    if (!m_planetProg->isLinked()) {
        m_planetProg->addAttribute("pos");
        m_planetProg->addAttribute("color");
        m_planetProg->addAttribute("uv");
        m_planetProg->addAttribute("hasTexture");
        m_planetProg->linkShaders();
    }
    m_planetProg->use();
    glUniform1i(m_planetProg->getUniformLocation("sampler"), 0);
    m_planetProg->unuse();

    m_batch.init();
    m_pbatch.init();
    m_overlay_batch.init();
    Logger::writeToLog("main screen initialized");
}

void MainGame::update() {
    static float dx = 0.618, dy = 1.618, dt = 0.01, ds = 1.01;
    if (m_debug) {
        if (InputManager::isPressed(SDLK_w)) {
            m_camera.translateCenter(0, dy);
        } if (InputManager::isPressed(SDLK_a)) {
            m_camera.translateCenter(-dx, 0);
        } if (InputManager::isPressed(SDLK_s)) {
            m_camera.translateCenter(0, -dy);
        } if (InputManager::isPressed(SDLK_d)) {
            m_camera.translateCenter(dx, 0);
        } if (InputManager::isPressed(SDLK_e)) {
            m_camera.rotate(-dt);
        } if (InputManager::isPressed(SDLK_q)) {
            m_camera.rotate(dt);
        } if (InputManager::getMouseWheelMotion() > 0) {
            m_camera.scaleDimensions(1./ds, 1./ds);
        } if (InputManager::getMouseWheelMotion() < 0) {
            m_camera.scaleDimensions(ds, ds);
        } if (InputManager::isPressed(SDLK_g)) {
            m_camera = Camera2D();
        }

        if (InputManager::justPressed(SDLK_RETURN)) {
            m_square_planet = !m_square_planet;
        }
    }

    if (InputManager::justPressed(SDLK_SPACE)) {
        m_debug = !m_debug;
        if (!m_debug) m_camera = Camera2D();
    } else if (InputManager::justPressed(SDLK_p)) {
        m_paused = !m_paused;
    }

    if (!m_paused) {
        m_time += 1/60.;
        m_world->Step(1./m_manager->getFPS(), 6, 2);
    }
}

void MainGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        if (m_debug) {
            vec2 center = m_camera.getCenter();
            m_batch.addGlyph(center - vec2(2.), center + vec2(2.), vec4(0,0,1,1),
                             ResourceManager::getTexture("circle.png").id, vec4(1,0,0,1), 0.0);
        }
    } m_batch.end();

    m_pbatch.begin(); {
        m_planet.render(m_pbatch);
        if (m_debug) {
            m_planet.render_debug(m_pbatch);
            debug_render_world(m_pbatch, m_world.get());
        }
    } m_pbatch.end();

    m_overlay_batch.begin(); {
        if (m_debug) {
            m_font->drawText(m_overlay_batch, "fps: " + to_string((int)m_manager->getFPS()), 
                             vec4(0, 0, 15, -5));
        }
        if (m_paused) {
            m_font->drawText(m_overlay_batch, "Paused", vec4(85, 0, 15, -5));
        }
    } m_overlay_batch.end();
        
    auto camera_matrix = m_camera.getCameraMatrix();
    m_planetProg->use(); {
        glUniformMatrix3fv(m_planetProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        glUniform1f(m_planetProg->getUniformLocation("planet_radius"), m_planet.getRadius());
        glUniform1f(m_planetProg->getUniformLocation("planet_height"), m_planet.getHeight());

        if (!m_square_planet) m_pbatch.render();
    } m_planetProg->unuse();

    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        
        if (m_square_planet) m_pbatch.render();
        m_batch.render();
    } m_simpleProg->unuse();

    m_overlayProg->use(); {
        m_overlay_batch.render();
    } m_overlayProg->unuse();
        
    m_window->swapBuffers();
}
