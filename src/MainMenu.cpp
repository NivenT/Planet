#include <nta/Logger.h>
#include <nta/InputManager.h>
#include <nta/ResourceManager.h>
#include <nta/SystemManager.h>

#include "MainMenu.h"
#include "defs.h"

using namespace std;
using namespace nta;
using namespace glm;

MainMenu::MainMenu() {
}

MainMenu::~MainMenu() {
}

void MainMenu::init() {
    Logger::writeToLog("Initializing MainMenu...");

    m_state = nta::ScreenState::RUNNING;
    //nta::Logger::writeToLog("Initializing GLSLPrograms...");
    m_simpleProg = nta::SystemManager::getGLSLProgram("shaders/simple2D");
    if (!m_simpleProg->isLinked()) {
        m_simpleProg->addAttribute("pos");
        m_simpleProg->addAttribute("color");
        m_simpleProg->addAttribute("uv");
        m_simpleProg->linkShaders();
    }

    m_simpleProg->use();
    glUniform1i(m_simpleProg->getUniformLocation("sampler"), 0);
    m_simpleProg->unuse();
    //nta::Logger::writeToLog("Initialized GLSLPrograms");

    m_hudBatch.init();
    m_font = nta::ResourceManager::getSpriteFont("resources/fonts/chintzy.ttf", 64);

    m_buttons[0].bounds = glm::vec4(-60, 20, 120, 20);
    m_buttons[0].backgroundColor = glm::vec4(0);
    m_buttons[0].name = "Play Game";
    m_buttons[0].next_index = MAINGAME_SCREEN_INDEX;

    Logger::writeToLog("Initialized MainMenu");
}

void MainMenu::update() {
    vec2 mouseCoords = m_hudCamera.mouseToGame(InputManager::getMouseCoords(), m_window->getDimensions());
    for (int i = 0; i < sizeof(m_buttons)/sizeof(Button); i++) {
        if (m_buttons[i].isPressed(mouseCoords) && InputManager::justPressed(SDL_BUTTON_LEFT)) {
            m_nextIndex = m_buttons[i].next_index;
            m_state = nta::ScreenState::SWITCH;
        }
    }
}

extern void collect_gl_errors();

void MainMenu::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static nta::GLTexture backgroundImage = nta::ResourceManager::getTexture("resources/images/background.png");
    m_hudBatch.begin();
    m_hudBatch.addGlyph(glm::vec4(-100,100,200,200), glm::vec4(0,0,1,1), backgroundImage.id, 1, glm::vec4(1,1,1,.2));
    m_font->drawText(m_hudBatch, m_window->getTitle(), glm::vec4(-80, 90, 160, 20));
    for (int i = 0; i < sizeof(m_buttons)/sizeof(Button); i++) {
        m_buttons[i].draw(m_font, m_hudBatch);
    }
    m_hudBatch.end();

    m_simpleProg->use();

    glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE, &m_hudCamera.getCameraMatrix()[0][0]);
    m_hudBatch.render();

    m_simpleProg->unuse();
    m_window->swapBuffers();

    collect_gl_errors();
}
