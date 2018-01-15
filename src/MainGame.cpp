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

using namespace std;
using namespace nta;
using namespace glm;

MainGame::MainGame() : m_time(0.), m_debug(false), m_square_planet(true), 
                       m_paused(false), m_draw_aabbs(true), m_soft_debug(true),
                       m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS) {
    m_planet = Planet::new_test();
    m_world = make_unique<b2World>(m_planet.getGravity());
    m_planet.add_to_world(m_world.get());

    m_player = new Player;
    m_objects.push_back(m_player);
    m_player->add_to_world(m_world.get());
}

MainGame::~MainGame() {
}

void MainGame::init() {
    Logger::writeToLog("Initializing main screen...");

    m_font = ResourceManager::getSpriteFont("resources/fonts/manilasansreg.otf");

    m_simpleProg = SystemManager::getGLSLProgram("shaders/simple2D");
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

    m_overlayProg = SystemManager::getGLSLProgram("shaders/overlay");
    if (!m_overlayProg->isLinked()) {
        m_overlayProg->addAttribute("pos");
        m_overlayProg->addAttribute("color");
        m_overlayProg->addAttribute("uv");
        m_overlayProg->addAttribute("hasTexture");
        m_overlayProg->linkShaders();
    }
    m_overlayProg->use();
    glUniform1i(m_overlayProg->getUniformLocation("sampler"), 0);
    m_overlayProg->unuse();

    m_planetProg = SystemManager::getGLSLProgram("planet", 
                                                 "shaders/planet.vert", "shaders/simple2D.frag");
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
    m_light_batch.init();
    m_overlay_batch.init();
    m_debug_batch.init();
    m_debug_sprite_batch.init();
    Logger::writeToLog("main screen initialized");
}

void MainGame::update() {
    static const float dx = 0.618, dy = 1.618, dt = 0.01, ds = 1.01;
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
        m_camera = m_debug ? Camera2D() : Camera2D(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS);
    } else if (InputManager::justPressed(SDLK_b)) {
        m_draw_aabbs = !m_draw_aabbs;
    } else if (InputManager::justPressed(SDLK_p)) {
        m_paused = !m_paused;
    } else if (InputManager::justPressed(SDLK_i)) {
        m_soft_debug = !m_soft_debug;
    }

    if (!m_paused && m_manager->getFPS() > 0.1) {
        m_time += 1/m_manager->getFPS();

        UpdateParams params;
        params.planet = &m_planet;
        for (auto& obj : m_objects) {
            obj->update(params);
        }
        m_world->Step(1./m_manager->getFPS(), 6, 2);

        if (!m_debug) m_camera.setCenter(m_player->getCenter());
    }
}

void MainGame::prepare_batches() {
    m_batch.begin(); {
        m_planet.render(m_batch);
    } m_batch.end();

    m_light_batch.begin(); {
        m_player->render(m_light_batch);
    } m_light_batch.end();

    m_overlay_batch.begin(); {
        if (m_debug) {
            m_font->drawText(m_overlay_batch, "fps: " + to_string((int)m_manager->getFPS()), 
                             vec4(0, 100, 15, 5));
            m_font->drawText(m_overlay_batch, "pos: " + to_string(m_camera.getCenter()),
                             vec4(0, 5, 20, 5));
            // This is useless, but I was curious
            m_font->drawText(m_overlay_batch, "time: " + to_string(m_time),
                             vec4(75, 5, 25, 5));
        }
        if (m_paused) {
            m_font->drawText(m_overlay_batch, "Paused", vec4(85, 100, 15, 5));
        }
    } m_overlay_batch.end();

    m_debug_batch.begin(); {
        if (m_debug || m_soft_debug) {
            debug_render_world(m_debug_batch, m_world.get(), m_draw_aabbs);
        }
        if (m_debug) {
            m_planet.render_debug(m_debug_batch);
        }
    } m_debug_batch.end();

    m_debug_sprite_batch.begin(); {
        if (m_debug) {
            vec2 center = m_camera.getCenter();
            GLTexture tex = ResourceManager::getTexture("resources/images/circle.png");

            m_debug_sprite_batch.addGlyph(center - vec2(2.), center + vec2(2.), vec4(0,0,1,1),
                                          tex.id, vec4(1,0,0,1), 1.0);
        }
    } m_debug_sprite_batch.end();
}

void MainGame::render_batches() {
    auto camera_matrix = m_camera.getCameraMatrix();
    m_planetProg->use(); {
        glUniformMatrix3fv(m_planetProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        glUniform1f(m_planetProg->getUniformLocation("is_light"), 0.0);
        glUniform1f(m_planetProg->getUniformLocation("planet_radius"), m_planet.getRadius());
        glUniform1f(m_planetProg->getUniformLocation("planet_height"), m_planet.getHeight());

        if (!m_square_planet) {
            m_batch.render();
            m_debug_batch.render();
        
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glUniform1f(m_planetProg->getUniformLocation("is_light"), 1.0);
            m_light_batch.render();
            glUniform1f(m_simpleProg->getUniformLocation("is_light"), 0.0);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    } m_planetProg->unuse();

    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        
        if (m_square_planet) {
            m_batch.render();
            m_debug_batch.render();
        
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glUniform1f(m_simpleProg->getUniformLocation("is_light"), 1.0);
            m_light_batch.render();
            glUniform1f(m_simpleProg->getUniformLocation("is_light"), 0.0);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        m_debug_sprite_batch.render();
    } m_simpleProg->unuse();

    m_overlayProg->use(); {
        m_overlay_batch.render();
    } m_overlayProg->unuse();
}

void MainGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prepare_batches();
    render_batches();

    m_window->swapBuffers();
}
