#include <iostream>

#include <nta/GLMConsoleOutput.h>
#include <nta/Logger.h>
#include <nta/SystemManager.h>
#include <nta/ResourceManager.h>
#include <nta/InputManager.h>
#include <nta/ScreenManager.h>
#include <nta/Vertex.h>
#include <nta/Random.h>

#include "MainGame.h"

using namespace std;
using namespace nta;
using namespace glm;

MainGame::MainGame() : m_debug(false), m_square_planet(false), 
                       m_paused(false), m_draw_aabbs(true), m_soft_debug(true),
                       m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS),
                       m_dev_mode(false) {
    m_planet = Planet::new_test();
    m_world = make_unique<b2World>(m_planet.getGravity());
}

MainGame::~MainGame() {
    offFocus();
}

void MainGame::onFocus() {
    m_planet.add_to_world(m_world.get());

    m_player = new Player;
    m_player->add_to_world(m_world.get(), CreationParams{});

    CreationParams item_params;
    item_params.planet = &m_planet;
    item_params.position = m_planet.getTileCenter(6, 35);

    Item* test_item = new Item("resources/images/stick.png", SMALL_ITEM_EXTENTS);
    test_item->add_to_world(m_world.get(), item_params);

    item_params.position = m_planet.getTileCenter(6, 10);
    Item* test_item2 = new Item("resources/images/rock.png", SMALL_ITEM_EXTENTS);
    test_item2->add_to_world(m_world.get(), item_params);

    CreationParams enemy_params;
    enemy_params.planet = &m_planet;
    enemy_params.position = m_planet.getTileCenter(6, 2);
    enemy_params.extents = ENEMY_UNIT_EXTENTS * vec2(3,1);
    Enemy* test_enemy = new Enemy("resources/images/shoe.png");

    test_enemy->add_to_world(m_world.get(), enemy_params);

    m_objects.push_back(m_player);
    m_objects.push_back(test_item);
    m_objects.push_back(test_item2);
    m_objects.push_back(test_enemy);

    m_state = ScreenState::RUNNING;
}

void MainGame::offFocus() {
    for (b2Body* curr = m_world->GetBodyList(); curr; curr = curr->GetNext()) {
        m_world->DestroyBody(curr);
    }

    for (auto object : m_objects) {
        delete object;
    }
    m_objects.clear();
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
        m_camera = Camera2D();
    }

    if (InputManager::justPressed(SDLK_RETURN)) {
        m_square_planet = !m_square_planet;
    }
}

void MainGame::dev_update() {
    if (InputManager::justPressed(SDLK_SPACE)) {
        m_debug = !m_debug;
        m_camera = m_debug ? Camera2D(vec2(0), glm::vec2(m_planet.getRadius())) : 
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
        //if (!m_dev_mode) m_debug = false;
    }

    if (!m_paused && m_manager->getFPS() > 0.1) {
        UpdateParams params;
        params.planet = &m_planet;
        params.world = m_world.get();

        for (auto& obj : m_objects) {
            obj->update(params);
        }
        m_world->Step(1./m_manager->getFPS(), 6, 2);

        if (!m_debug) m_camera.setCenter(m_player->getCenter());
    }
}

void MainGame::prepare_batches() {
    m_batch.begin();
    m_light_batch.begin();
    m_overlay_batch.begin();
    m_debug_batch.begin();
    m_debug_sprite_batch.begin();

    
    m_planet.render(m_batch);
    // +1 to skip the player
    for (auto it = m_objects.begin() + 1; it != m_objects.end(); ++it) {
        (*it)->render(m_batch);
    }
    m_player->render(m_light_batch);
    m_player->render_inventory(m_overlay_batch, m_font);
    m_player->render_health(m_batch);
    m_font->drawText(m_overlay_batch, "fps: " + to_string((int)m_manager->getFPS()), 
                         vec4(85, MEDIUM_TEXT_HEIGHT, 15, MEDIUM_TEXT_HEIGHT));

    if (m_paused) {
        m_font->drawText(m_overlay_batch, "Paused", vec4(85, 100, 15, MEDIUM_TEXT_HEIGHT));
    }
    if (m_dev_mode) {
        m_font->drawText(m_overlay_batch, "dev mode", vec4(40, 100, 20, MEDIUM_TEXT_HEIGHT));
    }
    if (m_debug) {
        m_planet.render_debug(m_debug_batch);
    }
    if (m_debug || m_soft_debug) {
        debug_render_world(m_debug_batch, m_world.get(), m_draw_aabbs);
    }

    m_batch.end();
    m_light_batch.end();
    m_overlay_batch.end();
    m_debug_batch.end();
    m_debug_sprite_batch.end();
}

// TODO: Somehow get depth working well
void MainGame::render_batches() {
    const auto camera_matrix = m_camera.getCameraMatrix();
    const float scale = m_camera.getDimensions().x;

    m_planetProg->use(); {
        glUniformMatrix3fv(m_planetProg->getUniformLocation("camera"), 1, GL_FALSE,
                           &camera_matrix[0][0]);
        glUniform1f(m_planetProg->getUniformLocation("is_light"), 0.0);
        glUniform1f(m_planetProg->getUniformLocation("normalized_planet_radius"), 
                    m_planet.getRadius()/scale);
        glUniform1f(m_planetProg->getUniformLocation("normalized_planet_height"), 
                    m_planet.getHeight()/scale);

        if (!m_square_planet) {
            m_batch.render();
            m_debug_batch.render();
        
            // TODO: Make lighting less hacky
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
