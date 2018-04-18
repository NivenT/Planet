#include <nta/Logger.h>
#include <nta/InputManager.h>
#include <nta/ResourceManager.h>
#include <nta/SystemManager.h>

#include "LevelEditor.h"
#include "defs.h"

using namespace std;
using namespace nta;
using namespace glm;

LevelEditor::LevelEditor() : Screen("Level Editor") {
}

LevelEditor::~LevelEditor() {
}

void LevelEditor::init() {
    Logger::writeToLog("Initializing LevelEditor...");

    m_prog = nta::SystemManager::getGLSLProgram("shaders/simple2D");
    check_error();
    if (!m_prog->isLinked()) {
        m_prog->addAttribute("pos");
        check_error();
        m_prog->addAttribute("color");
        check_error();
        m_prog->addAttribute("uv");
        check_error();
        m_prog->addAttribute("hasTexture");
        check_error();
        m_prog->linkShaders();
        check_error();
    }

    m_prog->use();
    glUniform1i(m_prog->getUniformLocation("sampler"), 0);
    m_prog->unuse();

    m_batch.init();
    m_font = nta::ResourceManager::getSpriteFont("resources/fonts/chintzy.ttf", 64);

    Logger::writeToLog("Initialized LevelEditor");
}

void LevelEditor::onFocus() {
    m_state = ScreenState::RUNNING;
}

void LevelEditor::offFocus() {
}

void LevelEditor::update() {
}

void LevelEditor::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin();
    m_font->drawText(m_batch, "Under Development...", glm::vec4(-75, 20, 150, 40));
    m_batch.end();

    m_prog->use();

    glUniformMatrix3fv(m_prog->getUniformLocation("camera"), 1, GL_FALSE, &m_camera.getCameraMatrix()[0][0]);
    m_batch.render();

    m_prog->unuse();
    m_window->swapBuffers();
}