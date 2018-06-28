#include <nta/Logger.h>
#include <nta/InputManager.h>
#include <nta/ResourceManager.h>
#include <nta/SystemManager.h>

#include <imgui/imgui_tabs.h>
#include <imgui/imgui_impl_sdl_gl3.h>

#include "WorldEditor.h"
#include "utils.h"
#include "defs.h"

#define GUI_CMD(cmd) if (cmd) { m_gui_focus = true; }

using namespace std;
using namespace nta;
using namespace glm;

WorldEditor::WorldEditor() : Screen("World Editor"), m_active_tile(vec4(.4, .7, .1, .8)),
                            m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS),
                            m_world(Planet::new_test()) {
    m_active_item.tex = "resources/images/stick.png";
    m_active_item.use_script = "scripts/stick.chai";

    m_world.init();
    m_world.set_flags(WORLD_DONT_DRAW_PLAYER_FLAG);
}

WorldEditor::~WorldEditor() {
}

vec2 WorldEditor::screen_to_game(crvec2 screen) const {
    return screenToGame(screen, m_window->getDimensions(), m_camera, 
                        m_world.get_planet(), m_square_planet);
}

void WorldEditor::init() {
    Logger::writeToLog("Initializing WorldEditor...");

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

    m_planet_prog = SystemManager::getGLSLProgram("planet", 
                                                  "shaders/planet.vert", "shaders/simple2D.frag");
    if (!m_planet_prog->isLinked()) {
        m_planet_prog->addAttribute("pos");
        m_planet_prog->addAttribute("color");
        m_planet_prog->addAttribute("uv");
        m_planet_prog->addAttribute("hasTexture");
        m_planet_prog->linkShaders();
    }
    m_planet_prog->use();
    glUniform1i(m_planet_prog->getUniformLocation("sampler"), 0);
    m_planet_prog->unuse();

    m_overlay_prog = SystemManager::getGLSLProgram("shaders/overlay");
    if (!m_overlay_prog->isLinked()) {
        m_overlay_prog->addAttribute("pos");
        m_overlay_prog->addAttribute("color");
        m_overlay_prog->addAttribute("uv");
        m_overlay_prog->addAttribute("hasTexture");
        m_overlay_prog->linkShaders();
    }
    m_overlay_prog->use();
    glUniform1i(m_overlay_prog->getUniformLocation("sampler"), 0);
    m_overlay_prog->unuse();

    m_batch.init();
    m_overlay_batch.init();
    m_font = nta::ResourceManager::getSpriteFont("resources/fonts/chintzy.ttf", 64);

    Logger::writeToLog("Initialized WorldEditor");
}

void WorldEditor::onFocus() {
    //m_world.init();

    m_state = ScreenState::RUNNING;
}

void WorldEditor::offFocus() {
    //m_world.destroy();
}

void WorldEditor::update() {
    static const float dx = 0.618, dy = 0.618, ds = 1.01;
    if (!m_gui_focus) {
        if (InputManager::isPressed(SDLK_w)) {
            m_camera.translateCenter(0, dy);
        } if (InputManager::isPressed(SDLK_a)) {
            m_camera.translateCenter(-dx, 0);
        } if (InputManager::isPressed(SDLK_s)) {
            m_camera.translateCenter(0, -dy);
        } if (InputManager::isPressed(SDLK_d)) {
            m_camera.translateCenter(dx, 0);
        } if (InputManager::getMouseWheelMotion() > 0) {
            m_camera.scaleDimensions(1./ds, 1./ds);
        } if (InputManager::getMouseWheelMotion() < 0) {
            m_camera.scaleDimensions(ds, ds);
        } if (InputManager::isPressed(SDLK_g)) {
            m_camera = Camera2D(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS);
        }
    }

    while (m_camera.getCenter().x > m_world.get_planet().getDimensions().x/2.f) {
        m_camera.translateCenter(-m_world.get_planet().getDimensions().x, 0);
    }
    while (m_camera.getCenter().x < -m_world.get_planet().getDimensions().x/2.f) {
        m_camera.translateCenter(m_world.get_planet().getDimensions().x, 0);
    }

    if (InputManager::justPressed(SDLK_RETURN)) {
        m_square_planet = !m_square_planet;
    } else if (InputManager::justPressed(SDLK_SPACE)) {
        m_gui_focus = !m_gui_focus;
    }


    vec2 mouse = InputManager::getMouseCoords();
    vec2 gui_region = m_window->getDimensions() * WORLDEDITOR_GUI_DIMS;
    if (!m_gui_focus && (mouse.x > gui_region.x || mouse.y > gui_region.y)) {
        vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
        auto coord = m_world.m_planet.getCoord(mouse);
        
        // Can I make this less redundant?
        if (InputManager::isPressed(SDL_BUTTON_LEFT)) {
            switch(m_curr_tab) {
            case GUI_TILE_TAB:
                if (0 <= coord.x && coord.x < m_world.m_planet.getDimensions()[0]) {
                    m_world.m_planet.m_tiles[coord[0]][coord[1]] = m_active_tile;
                }
                break;
            }
        } else if (InputManager::isPressed(SDL_BUTTON_RIGHT)) {
            switch(m_curr_tab) {
            case GUI_TILE_TAB:
                if (0 <= coord.x && coord.x < m_world.m_planet.getDimensions()[0]) {
                    m_active_tile = m_world.m_planet.m_tiles[coord[0]][coord[1]];
                }
                break;
            }
        }
    } else if (InputManager::isPressed(SDL_BUTTON_LEFT)) {
        m_gui_focus = true;
    }
}

void WorldEditor::prepare_batches() {
    m_batch.begin(); 
    m_overlay_batch.begin();

    auto text = m_gui_focus ? "GUI Focus" : "Planet Focus";
    m_font->drawText(m_overlay_batch, text, glm::vec4(80, 100, 20, MEDIUM_TEXT_HEIGHT));
    
    m_world.render(m_batch, m_overlay_batch, m_batch, m_font);

    vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
    switch(m_curr_tab) {
    case GUI_TILE_TAB:
        m_active_tile.render(m_batch, mouse + vec2(-TILE_SIZE, TILE_SIZE)/2.f);
        break;
    case GUI_ITEM_TAB: {
        Item* temp = new Item(m_active_item);

        CreationParams params;
        params.position = mouse;

        m_world.add_object(temp, params);
        temp->render(m_batch);
        m_world.remove_object(temp);
    } break;
    }
    
    m_overlay_batch.end();
    m_batch.end();
}

void WorldEditor::render_batches(const nta::Camera2D camera) {
    const auto matrix = camera.getCameraMatrix();
    const float scale = camera.getDimensions().x;

    m_prog->use(); {
        glUniformMatrix3fv(m_prog->getUniformLocation("camera"), 1, GL_FALSE, &matrix[0][0]);
        if (m_square_planet) m_batch.render();
    } m_prog->unuse();

    m_planet_prog->use(); {
        glUniformMatrix3fv(m_planet_prog->getUniformLocation("camera"), 1, GL_FALSE, &matrix[0][0]);
        glUniform1f(m_planet_prog->getUniformLocation("normalized_planet_radius"),
                    m_world.m_planet.getRadius()/scale);
        glUniform1f(m_planet_prog->getUniformLocation("normalized_planet_height"),
                    m_world.m_planet.getHeight()/scale);

        if (!m_square_planet) m_batch.render();
    } m_planet_prog->unuse();

    m_overlay_prog->use(); {
        m_overlay_batch.render();
    } m_overlay_prog->unuse();
}

void WorldEditor::render_miniworld() {
    const vec2 window_dims = m_window->getDimensions();
    const vec4 viewport(0.85*window_dims.x, 0.05*window_dims.y, 0.1*window_dims.x, 0.1*window_dims.y);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    const vec2 dims = m_square_planet ? m_world.m_planet.getDimensions() * vec2(0.55, 3):
                                        m_world.m_planet.getRadius() * vec2(2.1);
    const vec2 cen = m_square_planet ? vec2(0) : vec2(0);
    render_batches(Camera2D(cen, dims)); 
}

void WorldEditor::render_gui() {
    static char tile_tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char item_tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char item_use[GUI_TEXT_MAX_LENGTH] = "scripts/";

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    const auto size = m_window->getDimensions() * WORLDEDITOR_GUI_DIMS;
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
        ImGui::BeginTabBar("#Bar"); {
            ImGui::DrawTabsBackground();

            if (ImGui::AddTab("General")) {
                ImGui::Text("The quick brown fox jumps over the lazy dog");

                m_curr_tab = GUI_GENERAL_TAB;
            } if (ImGui::AddTab("Tile")){
                GUI_CMD(ImGui::ColorEdit4("color", (float*)&m_active_tile.color))
                GUI_CMD(ImGui::Checkbox("Destructable", &m_active_tile.destructable))
                GUI_CMD(ImGui::Checkbox("solid", &m_active_tile.solid))
                GUI_CMD(ImGui::Checkbox("active", &m_active_tile.active))
                GUI_CMD(ImGui::InputText("texture", tile_tex, GUI_TEXT_MAX_LENGTH))
                if (ImGui::Button("Update texture")) {
                    m_gui_focus = true;

                    auto tex = ResourceManager::getTexture(tile_tex);
                    if (tex.is_ok()) {
                        m_active_tile.tex = tex.get_data();
                    }
                }

                m_curr_tab = GUI_TILE_TAB;
            } if (ImGui::AddTab("Item")){
                GUI_CMD(ImGui::ColorEdit4("color", (float*)&m_active_item.color))
                GUI_CMD(ImGui::InputText("texture", item_tex, GUI_TEXT_MAX_LENGTH))
                GUI_CMD(ImGui::InputText("script", item_use, GUI_TEXT_MAX_LENGTH))
                if (ImGui::Button("Update texture")) {
                    m_gui_focus = true;
                    if (ResourceManager::getTexture(item_tex).is_ok()) {
                        m_active_item.tex = item_tex;
                    }
                } else if (ImGui::Button("Update script")) {
                    m_gui_focus = true;
                    m_active_item.use_script = item_use;
                }

                m_curr_tab = GUI_ITEM_TAB;
            } if (ImGui::AddTab("Enemy")){
                ImGui::Text("Working on it...");

                m_curr_tab = GUI_ENEMY_TAB;
            } if (ImGui::AddTab("Obstacle")){
                ImGui::Text("Working on it...");

                m_curr_tab = GUI_OBSTACLE_TAB;
            }
        } ImGui::EndTabBar();
    } ImGui::End();
}

void WorldEditor::render() {
    const vec2 window_dims = m_window->getDimensions();
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_dims.x, window_dims.y);

    prepare_batches();
    render_batches(m_camera);
    render_miniworld();
    render_gui();

    m_window->swapBuffers();
}
