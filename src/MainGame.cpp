#include <iostream>

#include "nta/Logger.h"
#include "nta/SystemManager.h"
#include "nta/ResourceManager.h"
#include "nta/InputManager.h"
#include "nta/ScreenManager.h"
#include "nta/Vertex.h"
#include "nta/Random.h"
#include "nta/GLMConsoleOutput.h"

#include "MainGame.h"

using namespace std;
using namespace nta;
using namespace glm;

MainGame::MainGame() : m_time(0.), m_debug(true) {
    m_planet = Planet::new_test();
}

MainGame::~MainGame() {
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

    m_batch.init();
    m_pbatch.init();
    m_overlay_batch.init();
    Logger::writeToLog("main screen initialized");
}

void MainGame::update() {
    static float dx = 0.618, dy = 1.618, dt = 0.01;
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
    } if (InputManager::isPressed(SDLK_g)) {
        m_camera = Camera2D();
    }

    if (InputManager::justPressed(SDLK_SPACE)) {
        m_debug = !m_debug;
    }

    m_time += 1/60.;
}

void MainGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        if (m_debug) {
            vec2 center = m_camera.getCenter();
            m_batch.addGlyph(center - vec2(2.), center + vec2(2.), vec4(0,0,1,1),
                             ResourceManager::getTexture("circle.png").id, vec4(1,0,0,1));
        }
    } m_batch.end();

    m_pbatch.begin(); {
        m_planet.render(m_pbatch);
        if (m_debug) m_planet.render_debug(m_pbatch);
    } m_pbatch.end();

    m_overlay_batch.begin(); {
        if (m_debug) {
            m_font->drawText(m_overlay_batch, "fps: " + to_string((int)m_manager->getFPS()), 
                             vec4(0, 0, 15, -5));
        }
    } m_overlay_batch.end();
        
    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_pbatch.render();
        m_batch.render();
    } m_simpleProg->unuse();

    m_overlayProg->use(); {
        m_overlay_batch.render();
    } m_overlayProg->unuse();
        
    m_window->swapBuffers();
}
