#include <nta/Logger.h>
#include <nta/InputManager.h>
#include <nta/ResourceManager.h>
#include <nta/SystemManager.h>

#include <imgui/imgui_tabs.h>
#include <imgui/imgui_impl_sdl_gl3.h>

#include "WorldEditor.h"
#include "utils.h"
#include "defs.h"

#define GUI_CMD(cmd) if (cmd) { gui_active = true; }

using namespace std;
using namespace nta;
using namespace glm;

WorldEditor::WorldEditor() : Screen("World Editor"), clear_color(0), m_active_tile(vec4(1)),
                            m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS) {
    m_planet = Planet::new_test();
}

WorldEditor::~WorldEditor() {
}

// Dependent on code in planet.vert
vec2 WorldEditor::screen_to_game(crvec2 screen) const {
    return screenToGame(screen, m_window->getDimensions(), m_camera, m_planet, m_square_planet);
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

    m_batch.init();
    m_font = nta::ResourceManager::getSpriteFont("resources/fonts/chintzy.ttf", 64);

    m_window->resize(1000, 600);

    Logger::writeToLog("Initialized WorldEditor");
}

void WorldEditor::onFocus() {
    m_state = ScreenState::RUNNING;
}

void WorldEditor::offFocus() {
}

void WorldEditor::update() {
    if (gui_active) return;

    static const float dx = 0.618, dy = 0.618, ds = 1.01;
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

    while (m_camera.getCenter().x > m_planet.getDimensions().x/2.f) {
        m_camera.translateCenter(-m_planet.getDimensions().x, 0);
    }
    while (m_camera.getCenter().x < -m_planet.getDimensions().x/2.f) {
        m_camera.translateCenter(m_planet.getDimensions().x, 0);
    }

    if (InputManager::justPressed(SDLK_RETURN)) {
        m_square_planet = !m_square_planet;
    } else if (InputManager::isPressed(SDL_BUTTON_LEFT)) {
        vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
        auto coord = m_planet.getCoord(mouse);
        m_planet.m_tiles[coord[0]][coord[1]] = m_active_tile;
    } else if (InputManager::isPressed(SDL_BUTTON_RIGHT)) {
        vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
        auto coord = m_planet.getCoord(mouse);
        m_active_tile = m_planet.m_tiles[coord[0]][coord[1]];
    }
}

void WorldEditor::prepare_batches() {
    m_batch.begin(); 

    //m_font->drawText(m_batch, "Under Development...", glm::vec4(-75, 20, 150, 40));
    m_planet.render(m_batch);

    vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
    m_active_tile.render(m_batch, mouse + vec2(-TILE_SIZE, TILE_SIZE)/2.f);
    
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
                    m_planet.getRadius()/scale);
        glUniform1f(m_planet_prog->getUniformLocation("normalized_planet_height"),
                    m_planet.getHeight()/scale);

        if (!m_square_planet) m_batch.render();
    } m_planet_prog->unuse();
}

void WorldEditor::render_miniworld() {
    const vec2 window_dims = m_window->getDimensions();
    const vec4 viewport(0.85*window_dims.x, 0.05*window_dims.y, 0.1*window_dims.x, 0.1*window_dims.y);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    const vec2 dims = m_square_planet ? m_planet.getDimensions() * vec2(0.55, 3):
                                        m_planet.getRadius() * vec2(2.1);
    const vec2 cen = m_square_planet ? vec2(0) : vec2(0);
    render_batches(Camera2D(cen, dims)); 
}

void WorldEditor::render_gui() {
    static string tile_tex = "resources/images/";
    tile_tex.reserve(GUI_TEXT_MAX_LENGTH);

    gui_active = false;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    const auto size = m_window->getDimensions() * WORLDEDITOR_GUI_DIMS;
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
        ImGui::BeginTabBar("#Bar"); {
            ImGui::DrawTabsBackground();

            if (ImGui::AddTab("General")) {
                ImGui::Text("The quick brown fox jumps over the lazy dog");
                ImGui::ColorEdit3("clear color", (float*)&clear_color);
            } if (ImGui::AddTab("Tile")){
                GUI_CMD(ImGui::ColorEdit4("color", (float*)&m_active_tile.color))
                GUI_CMD(ImGui::Checkbox("Destructable", &m_active_tile.destructable))
                GUI_CMD(ImGui::Checkbox("solid", &m_active_tile.solid))
                GUI_CMD(ImGui::Checkbox("active", &m_active_tile.active))
                GUI_CMD(ImGui::InputText("texture", (char*)tile_tex.data(), GUI_TEXT_MAX_LENGTH))
                if (ImGui::Button("Update texture")) {
                    gui_active = true;
                    m_active_tile.tex = ResourceManager::getTexture(tile_tex);
                }
            } if (ImGui::AddTab("Item")){
                ImGui::Text("Working on it...");
            } if (ImGui::AddTab("Enemy")){
                ImGui::Text("Working on it...");
            } if (ImGui::AddTab("Obstacle")){
                ImGui::Text("Working on it...");
            }
        } ImGui::EndTabBar();
    } ImGui::End();
}

void WorldEditor::render() {
    const vec2 window_dims = m_window->getDimensions();
    
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_dims.x, window_dims.y);

    prepare_batches();
    render_batches(m_camera);
    render_miniworld();
    render_gui();

    m_window->swapBuffers();
}
