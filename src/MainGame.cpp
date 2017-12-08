#include "nta/Logger.h"
#include "nta/SystemManager.h"
#include "nta/ResourceManager.h"
#include "nta/Vertex.h"
#include "nta/Random.h"

#include "MainGame.h"

using namespace std;
using namespace nta;
using namespace glm;

MainGame::MainGame() {
}

MainGame::~MainGame() {
}

void MainGame::init() {
    Logger::writeToLog("Initializing main screen...");

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
}

void MainGame::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_batch.begin(); {
		} m_batch.end();

    static vec4 color(Random::randFloat(), Random::randFloat(), Random::randFloat(), 1.0);
		m_pbatch.begin(); {
        vector<Vertex2D> verts;
        for (int i = 0; i < 6; i++) {
            verts.emplace_back(10.f * vec2(cos(i * M_PI/3), sin(i * M_PI/3)), color);
        }
        m_pbatch.addPrimitive(begin(verts), end(verts), 0);
		} m_pbatch.end();
		
    m_simpleProg->use(); {
        glUniformMatrix3fv(m_simpleProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &m_camera.getCameraMatrix()[0][0]);
        m_batch.render();
        m_pbatch.render();
		} m_simpleProg->unuse();
		
    m_window->swapBuffers();
}
