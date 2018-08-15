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
                             m_camera(DEFAULT_CAMERA_CENTER, DEFAULT_CAMERA_DIMENSIONS) {
    m_active_item.tex = "resources/images/stick.png";
    m_active_item.use_script = "scripts/stick.chai";

    m_active_enemy.tex = "resources/images/shoe.png";
    m_active_enemy.update_script = "scripts/shoe.chai";

    m_active_spawner.tex = "resources/images/shoe_spawner.png";
    m_active_spawner.spawn = m_active_enemy;
    m_active_spawner.extents.x = 1.5f;

    m_world.planet = Planet::new_test();
}

WorldEditor::~WorldEditor() {
}

WorldParams* WorldEditor::get_world() {
    return &m_world;
}

vec2 WorldEditor::screen_to_game(crvec2 screen) const {
    return screenToGame(screen, m_window->getDimensions(), m_camera, 
                        m_world.planet, m_square_planet);
}

bool WorldEditor::gui_in_use() const {
    return m_gui_focus /* && m_gui_active */;
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
    m_dbatch.init();
    m_font = nta::ResourceManager::getSpriteFont("resources/fonts/chintzy.ttf", 64);

    Logger::writeToLog("Initialized WorldEditor");
}

void WorldEditor::onFocus(const ScreenSwitchInfo& info) {
    m_state = ScreenState::RUNNING;
}

void WorldEditor::offFocus() {
    m_switchData = &m_world;
}

void WorldEditor::update_camera() {
    static const float dx = 0.618, dy = 0.618, ds = 1.01;
    if (!gui_in_use()) {
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

    while (m_camera.getCenter().x > m_world.planet.getDimensions().x/2.f) {
        m_camera.translateCenter(-m_world.planet.getDimensions().x, 0);
    }
    while (m_camera.getCenter().x < -m_world.planet.getDimensions().x/2.f) {
        m_camera.translateCenter(m_world.planet.getDimensions().x, 0);
    }
}

void WorldEditor::update_tile_tab(crvec2 mouse, const ivec2& coord) {
    if (InputManager::isPressed(SDL_BUTTON_LEFT)) {
        if (0 <= coord.x && coord.x < m_world.planet.getDimensions()[0]) {
            m_world.planet.m_tiles[coord[0]][coord[1]] = m_active_tile;
        }
    } else if (InputManager::isPressed(SDL_BUTTON_RIGHT)) {
        if (0 <= coord.x && coord.x < m_world.planet.getDimensions()[0]) {
            m_active_tile = m_world.planet.m_tiles[coord[0]][coord[1]];
        }
    }
}

void WorldEditor::update_item_tab(crvec2 mouse) {
    if (InputManager::justPressed(SDL_BUTTON_LEFT)) {
        m_active_item.position = mouse;
        m_active_item.planet = &m_world.planet;
        m_world.items.push_back(m_active_item);
    }
}

void WorldEditor::update_enemy_tab(crvec2 mouse) {
    if (InputManager::justPressed(SDL_BUTTON_LEFT)) {
        m_active_enemy.position = mouse;
        m_active_enemy.planet = &m_world.planet;
        m_world.enemies.push_back(m_active_enemy);
    }
}

void WorldEditor::update_spawner_tab(crvec2 mouse) {
    if (InputManager::justPressed(SDL_BUTTON_LEFT)) {
        m_active_spawner.position = mouse;
        m_active_spawner.planet = &m_world.planet;
        m_world.spawners.push_back(m_active_spawner);
    }
}

void WorldEditor::update() {
    update_camera();
    if (InputManager::justPressed(SDLK_RETURN)) {
        m_square_planet = !m_square_planet;
    } else if (InputManager::justPressed(SDLK_SPACE)) {
        m_gui_focus = !m_gui_focus;
    }

    vec2 mouse = InputManager::getMouseCoords();
    vec2 gui_region = m_window->getDimensions() * WORLDEDITOR_GUI_DIMS;
    if (m_gui_active && mouse.x <= gui_region.x && mouse.y <= gui_region.y) {
        m_gui_focus = true;
    } else if (!gui_in_use()) {
        vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
        auto coord = m_world.planet.getCoord(mouse);

        switch(m_curr_tab) {
            case GUI_TILE_TAB: update_tile_tab(mouse, coord); break;
            case GUI_ITEM_TAB: update_item_tab(mouse); break;
            case GUI_ENEMY_TAB: update_enemy_tab(mouse); break;
            case GUI_SPAWNER_TAB: update_spawner_tab(mouse); break;
        }
    }
}

void WorldEditor::prepare_batches() {
    m_batch.begin(); 
    m_overlay_batch.begin();
    m_dbatch.begin();

    auto text = m_gui_focus ? "GUI Focus" : "Editor Focus";
    m_font->drawText(m_overlay_batch, text, glm::vec4(80, 100, 20, MEDIUM_TEXT_HEIGHT));

    World temp_world(m_world);
    temp_world.set_flags(WORLD_DONT_DRAW_PLAYER_FLAG);

    vec2 mouse = screen_to_game(InputManager::getMouseCoordsStandard(m_window->getHeight()));
    // I really don't like having a switch statement here
    switch(m_curr_tab) {
        case GUI_ITEM_TAB: {
            m_active_item.position = mouse;
            temp_world.add_object(new Item(m_active_item), m_active_item);
        } break;
        case GUI_ENEMY_TAB: {
            m_active_enemy.position = mouse;
            temp_world.add_object(new Enemy(m_active_enemy), m_active_enemy);
        } break;
        case GUI_SPAWNER_TAB: {
            m_active_spawner.position = mouse;
            temp_world.add_object(new Spawner(m_active_spawner), m_active_spawner);
        } break;
    }
    temp_world.render(m_batch, m_overlay_batch, m_font);
    switch(m_curr_tab) {
        case GUI_TILE_TAB:
            m_active_tile.render(m_batch, mouse + vec2(-TILE_SIZE, TILE_SIZE)/2.f);
            break;
    }
    m_font->drawText(m_batch, "Player\nSpawn",
                     glm::vec4(PLAYER_INIT_POS.x,PLAYER_INIT_POS.y,PLAYER_DIMS));

    m_world.planet.render_debug(m_dbatch);

    m_dbatch.end();
    m_overlay_batch.end();
    m_batch.end();

    temp_world.destroy();
}

void WorldEditor::render_batches(const nta::Camera2D camera, bool debug) {
    const auto matrix = camera.getCameraMatrix();
    const float scale = camera.getDimensions().x;

    m_prog->use(); {
        glUniformMatrix3fv(m_prog->getUniformLocation("camera"), 1, GL_FALSE, &matrix[0][0]);
        if (m_square_planet) {
            m_batch.render();
            if (debug) m_dbatch.render();
        }
    } m_prog->unuse();

    m_planet_prog->use(); {
        glUniformMatrix3fv(m_planet_prog->getUniformLocation("camera"), 1, GL_FALSE, &matrix[0][0]);
        glUniform1f(m_planet_prog->getUniformLocation("normalized_planet_radius"),
                    m_world.planet.getRadius()/scale);
        glUniform1f(m_planet_prog->getUniformLocation("normalized_planet_height"),
                    m_world.planet.getHeight()/scale);

        if (!m_square_planet) {
            m_batch.render();
            if (debug) m_dbatch.render();
        }
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

    const vec2 dims = m_square_planet ? m_world.planet.getDimensions() * vec2(0.55, 3):
                                        m_world.planet.getRadius() * vec2(2.1);
    const vec2 cen = m_square_planet ? vec2(0) : vec2(0);
    render_batches(Camera2D(cen, dims), false); 
}

void WorldEditor::render_planet_tab() {
    static const string folder = "resources/data/planets/";
    static char name[GUI_TEXT_MAX_LENGTH] = "planet";

    GUI_CMD(ImGui::SliderInt("sea level", &m_world.planet.m_sea_level, 0, 
                             m_world.planet.rows-1, "%.0f tiles from the top"))
    if (ImGui::SliderInt("radius", &m_world.planet.rows, 10, 100, "%.0f tiles")) {
        m_gui_focus = true;
        m_world.planet.set_num_rows(m_world.planet.rows);
    }
    GUI_CMD(ImGui::SliderFloat2("gravity", (float*)&m_world.planet.m_gravity,
                                PLANET_MIN_GRAVITY, PLANET_MAX_GRAVITY))
    GUI_CMD(ImGui::InputText("World Name", name, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Save")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_world.save(folder + name + extension);
    } 
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_world.clear();
        m_world = WorldParams::load(utils::Json::from_file(folder + name + extension));     
    }

    m_curr_tab = GUI_PLANET_TAB;
}

void WorldEditor::render_tile_tab() {
    static char tile_tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";

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
}

void WorldEditor::render_item_tab() {
    static const string folder = "resources/data/items/";
    static char tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char use[GUI_TEXT_MAX_LENGTH] = "scripts/";
    static char name[GUI_TEXT_MAX_LENGTH] = "stick";

    GUI_CMD(ImGui::ColorEdit4("color", (float*)&m_active_item.color))
    GUI_CMD(ImGui::SliderFloat2("extents", (float*)&m_active_item.extents,
                                MIN_ITEM_SIZE.x, MAX_ITEM_SIZE.x))
    GUI_CMD(ImGui::InputText("texture", tex, GUI_TEXT_MAX_LENGTH))
    GUI_CMD(ImGui::InputText("script", use, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Update texture")) {
        m_gui_focus = true;
        if (ResourceManager::getTexture(tex).is_ok()) {
            m_active_item.tex = tex;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Update script")) {
        m_gui_focus = true;
        // TODO: Check file exists
        m_active_item.use_script = use;
    }

    GUI_CMD(ImGui::InputText("name", name, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Save")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_active_item.save(folder + name + extension);
    } 
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_active_item = ItemParams::load(utils::Json::from_file(folder+name+extension));    
    }

    m_curr_tab = GUI_ITEM_TAB;
}

void WorldEditor::render_enemy_tab() {
    static const string folder = "resources/data/enemies/";
    static char tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char script[GUI_TEXT_MAX_LENGTH] = "scripts/";
    static char name[GUI_TEXT_MAX_LENGTH] = "shoe";

    GUI_CMD(ImGui::ColorEdit4("color", (float*)&m_active_enemy.color))
    GUI_CMD(ImGui::SliderFloat2("extents", (float*)&m_active_enemy.extents,
                                ENEMY_MIN_EXTENTS, ENEMY_MAX_EXTENTS))
    GUI_CMD(ImGui::SliderFloat("health", &m_active_enemy.init_health,
                               ENEMY_MIN_INIT_HEALTH, ENEMY_MAX_INIT_HEALTH))
    GUI_CMD(ImGui::InputText("texture", tex, GUI_TEXT_MAX_LENGTH))
    GUI_CMD(ImGui::InputText("script", script, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Update texture")) {
        m_gui_focus = true;
        if (ResourceManager::getTexture(tex).is_ok()) {
            m_active_enemy.tex = tex;
        }
    } 
    ImGui::SameLine();
    if (ImGui::Button("Update script")) {
        m_gui_focus = true;
        m_active_enemy.update_script = script;
    }

    GUI_CMD(ImGui::InputText("name", name, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Save")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_active_enemy.save(folder + name + extension);
    } ImGui::SameLine();
    if (ImGui::Button("Load")) {
        m_gui_focus = true;

        string extension = utils::ends_with(name, ".json") ? "" : ".json";
        m_active_enemy = EnemyParams::load(utils::Json::from_file(folder+name+extension));    
    }

    m_curr_tab = GUI_ENEMY_TAB;
}

void WorldEditor::render_spawner_tab() {
    static const string folder = "resources/data/spawners/";
    static char spawn_tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char spawn_script[GUI_TEXT_MAX_LENGTH] = "scripts/";
    static char spawner_tex[GUI_TEXT_MAX_LENGTH] = "resources/images/";
    static char spawner_script[GUI_TEXT_MAX_LENGTH] = "scripts/";
    static char spawner_name[GUI_TEXT_MAX_LENGTH] = "shoe_spawner";


    ImGui::Text("Spawner data");
    ImGui::Text("===================================");
    GUI_CMD(ImGui::ColorEdit4("color##Spawner", (float*)&m_active_spawner.color))
    GUI_CMD(ImGui::SliderFloat2("extents##Spawner", (float*)&m_active_spawner.extents,
                                ENEMY_MIN_EXTENTS, ENEMY_MAX_EXTENTS))
    GUI_CMD(ImGui::SliderFloat("health##Spawner", &m_active_spawner.init_health,
                               ENEMY_MIN_INIT_HEALTH, ENEMY_MAX_INIT_HEALTH))
    GUI_CMD(ImGui::InputText("texture##Spawner", spawner_tex, GUI_TEXT_MAX_LENGTH))
    GUI_CMD(ImGui::InputText("script##Spawner", spawner_script, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Update texture##Spawner")) {
        m_gui_focus = true;
        if (ResourceManager::getTexture(spawner_tex).is_ok()) {
            m_active_spawner.tex = spawner_tex;
        }
    } 
    ImGui::SameLine();
    if (ImGui::Button("Update script##Spawner")) {
        m_gui_focus = true;
        m_active_spawner.spawn.update_script = spawner_script;
    }

    GUI_CMD(ImGui::InputText("name", spawner_name, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Save")) {
        m_gui_focus = true;

        string extension = utils::ends_with(spawner_name, ".json") ? "" : ".json";
        m_active_spawner.save(folder + spawner_name + extension);
    } 
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        m_gui_focus = true;

        string extension = utils::ends_with(spawner_name, ".json") ? "" : ".json";
        m_active_spawner = SpawnerParams::load(utils::Json::from_file(folder+spawner_name+extension));    
    }

    ImGui::Text("\nSpawn data");
    ImGui::Text("===================================");
    GUI_CMD(ImGui::ColorEdit4("color##Spawn", (float*)&m_active_spawner.spawn.color))
    GUI_CMD(ImGui::SliderFloat2("extents##Spawn", (float*)&m_active_spawner.spawn.extents,
                                ENEMY_MIN_EXTENTS, ENEMY_MAX_EXTENTS))
    GUI_CMD(ImGui::SliderFloat("health##Spawn", &m_active_spawner.spawn.init_health,
                               ENEMY_MIN_INIT_HEALTH, ENEMY_MAX_INIT_HEALTH))
    GUI_CMD(ImGui::InputText("texture##Spawn", spawn_tex, GUI_TEXT_MAX_LENGTH))
    GUI_CMD(ImGui::InputText("script##Spawn", spawn_script, GUI_TEXT_MAX_LENGTH))
    if (ImGui::Button("Update texture##Spawn")) {
        m_gui_focus = true;
        if (ResourceManager::getTexture(spawn_tex).is_ok()) {
            m_active_spawner.spawn.tex = spawn_tex;
        }
    } 
    ImGui::SameLine();
    if (ImGui::Button("Update script##Spawn")) {
        m_gui_focus = true;
        m_active_spawner.spawn.update_script = spawn_script;
    }

    m_curr_tab = GUI_SPAWNER_TAB;
}

void WorldEditor::render_obstacle_tab() {
    ImGui::Text("Working on it...");

    m_curr_tab = GUI_OBSTACLE_TAB;
}

// TODO: Break into separate functions?
// TODO: Use ImGui file browser
void WorldEditor::render_gui() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    const auto size = m_window->getDimensions() * WORLDEDITOR_GUI_DIMS;
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    
    ImGui::Begin("Test Window", &m_gui_active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); {
        ImGui::BeginTabBar("#Bar"); {
            ImGui::DrawTabsBackground();
            if (ImGui::AddTab("World")) {
                render_planet_tab();
            } if (ImGui::AddTab("Tile")) {
                render_tile_tab();
            } if (ImGui::AddTab("Item")) {
                render_item_tab();
            } if (ImGui::AddTab("Enemy")) {
                render_enemy_tab();
            } if (ImGui::AddTab("Spawner")) {
                render_spawner_tab();
            } if (ImGui::AddTab("Obstacle")) {
                render_obstacle_tab();
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
    render_batches(m_camera, true);
    render_miniworld();
    render_gui();

    m_window->swapBuffers();
}
