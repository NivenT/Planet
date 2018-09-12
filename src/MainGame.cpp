#include <iostream>

#include <nta/Logger.h>
#include <nta/SystemManager.h>
#include <nta/ResourceManager.h>
#include <nta/InputManager.h>
#include <nta/ScreenManager.h>
#include <nta/Vertex.h>
#include <nta/Random.h>

#include "MainGame.h"
#include "ChaiManager.h"
#include "utils.h"

using namespace std;
using namespace nta;
using namespace glm; 

MainGame::MainGame() : m_debug(false), m_square_planet(false), 
                       m_paused(false), m_draw_aabbs(true), m_soft_debug(false),
                       m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS),
                       m_dev_mode(false), Screen("Main Game") {
}

MainGame::~MainGame() {
}

vec2 MainGame::getMouse() const {
    return screenToGame(InputManager::getMouseCoordsStandard(m_window->getHeight()),
                        m_window->getDimensions(), m_camera, m_world->get_planet(), 
                        m_square_planet);
}

void MainGame::onFocus(const ScreenSwitchInfo& info) {
    if (!info.data) Logger::writeErrorToLog("Tried starting game with empty World",
                                             nta::ErrorType::INVALID_VALUE);
    m_world = new World(*(WorldParams*)info.data);
    m_state = ScreenState::RUNNING;
}

void MainGame::offFocus() {
    delete m_world;
    m_switchData = nullptr;
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
    m_overlay_batch.init();
    m_debug_batch.init();

    ChaiManager::add(chaiscript::fun([&](){return getMouse();}), "getMouse");

    Logger::writeToLog("main screen initialized");
}

void MainGame::debug_update() {
    static const float dx = 0.618, dy = 1.618, dt = 0.01, ds = 1.01;
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
        m_camera = Camera2D(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS);
    }

    if (InputManager::justPressed(SDLK_RETURN)) {
        m_square_planet = !m_square_planet;
    }
}

void MainGame::dev_update() {
    if (InputManager::justPressed(SDLK_SPACE)) {
        m_debug = !m_debug;
        m_camera = m_debug ? Camera2D(vec2(0), glm::vec2(m_world->get_planet().getRadius())) : 
                             Camera2D(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS);
    } else if (InputManager::justPressed(SDLK_b)) {
        m_draw_aabbs = !m_draw_aabbs;
    } else if (InputManager::justPressed(SDLK_i)) {
        m_soft_debug = !m_soft_debug;
    }
}

void MainGame::update() {
    if (m_dev_mode) {
        dev_update();
    } if (m_debug) {
        debug_update();
    }

    if (InputManager::justPressed(SDLK_p)) {
        m_paused = !m_paused;
    } if (InputManager::justPressed(SDLK_F1)) {
        m_dev_mode = !m_dev_mode;
    }

    if (!m_paused && m_manager->getFPS() > 0.1) {
        UpdateParams params;
        params.dt = 1./m_manager->getFPS();

        if (m_world->update(params)) {
            m_state = ScreenState::SWITCH_ESC;
        }
    }
    if (!m_debug) m_camera.setCenter(m_world->get_player_center());
}

void MainGame::prepare_batches() {
    m_batch.begin();
    m_overlay_batch.begin();
    m_debug_batch.begin();

    m_world->render(m_batch, m_overlay_batch, m_font);
    m_font->drawText(m_overlay_batch, "FPS: " + to_string((int)m_manager->getFPS()), 
                         vec4(0, MEDIUM_TEXT_HEIGHT, 15, MEDIUM_TEXT_HEIGHT));
    if (m_paused) {
        m_font->drawText(m_overlay_batch, "Paused", vec4(85, 100, 15, MEDIUM_TEXT_HEIGHT));
    }
    if (m_dev_mode) {
        m_font->drawText(m_overlay_batch, "dev mode", vec4(40, 100, 20, MEDIUM_TEXT_HEIGHT));
    }
    if (m_debug) {
        m_world->render_debug(m_debug_batch);
    }
    if (m_debug || m_soft_debug) {
        debug_render_world(m_debug_batch, m_world->get_b2World(), m_draw_aabbs);
    }

    m_batch.end();
    m_overlay_batch.end();
    m_debug_batch.end();
}

// TODO: Somehow get depth working well
void MainGame::render_batches() {
    const auto camera_matrix = m_camera.getCameraMatrix();
    const float scale = m_camera.getDimensions().x;

    m_planetProg->use(); {
        glUniformMatrix3fv(m_planetProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        glUniform1f(m_planetProg->getUniformLocation("normalized_planet_radius"), 
                    m_world->get_planet().getRadius()/scale);
        glUniform1f(m_planetProg->getUniformLocation("normalized_planet_height"), 
                    m_world->get_planet().getHeight()/scale);

        if (!m_square_planet) {
            m_batch.render();
            m_debug_batch.render();
        }
    } m_planetProg->unuse();

    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        
        if (m_square_planet) {
            m_batch.render();
            m_debug_batch.render();
        }
    } m_simpleProg->unuse();

    m_overlayProg->use(); {
        m_overlay_batch.render();
    } m_overlayProg->unuse();
}

void MainGame::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_window->getDimensions().x, m_window->getDimensions().y);

    prepare_batches();
    render_batches();

    m_window->swapBuffers();
}
