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

MainGame::MainGame() : m_time(0.), m_debug(false) {
    m_planet = Planet::new_test();
}

MainGame::~MainGame() {
}

void MainGame::init() {
    Logger::writeToLog("Initializing main screen...");

    m_font = ResourceManager::getSpriteFont("manilasansreg.otf");

    m_simpleProg = SystemManager::getGLSLProgram("Simple2D");
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

    m_batch.init();
    m_pbatch.init();
    Logger::writeToLog("main screen initialized");
}

void MainGame::update() {
    static float dx = 0.618, dy = 1.618, dt = 0.01;
    if (nta::InputManager::isPressed(SDLK_w)) {
        m_camera.translateCenter(0, dy);
    } if (nta::InputManager::isPressed(SDLK_a)) {
        m_camera.translateCenter(-dx, 0);
    } if (nta::InputManager::isPressed(SDLK_s)) {
        m_camera.translateCenter(0, -dy);
    } if (nta::InputManager::isPressed(SDLK_d)) {
        m_camera.translateCenter(dx, 0);
    } if (nta::InputManager::isPressed(SDLK_e)) {
        m_camera.rotate(-dt);
    } if (nta::InputManager::isPressed(SDLK_q)) {
        m_camera.rotate(dt);
    }

    if (nta::InputManager::justPressed(SDLK_SPACE)) {
        m_debug = !m_debug;
    }

    m_time += 1/60.;
}

void MainGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
        if (m_debug) {
            m_font->drawText(m_batch, "fps: " + to_string((int)m_manager->getFPS()), 
                             vec4(m_camera.getTopLeft(), m_camera.getDimensions()*vec2(0.3, 0.1)));
        }
    } m_batch.end();

    m_pbatch.begin(); {
        m_planet.render(m_pbatch);
        if (m_debug) m_planet.render_debug(m_pbatch);
    } m_pbatch.end();
        
    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_batch.render();
        m_pbatch.render();
    } m_simpleProg->unuse();
        
    m_window->swapBuffers();
}
