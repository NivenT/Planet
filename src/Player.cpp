#include <nta/InputManager.h>
#include <nta/CallbackManager.h>
#include <nta/ResourceManager.h>

#include "Player.h"
#include "Enemy.h"

using namespace std;
using namespace glm;
using namespace nta;

Player::Player(uint16_t type) : 
    Agent(PLAYER_MAX_SPEED, PLAYER_COLOR, PLAYER_INIT_HEALTH, type | PLAYER_TYPE),
    m_inventory_event_id(0) {
    set_flags(AGENT_STATE_SHOW_HEALTH);
    m_anim = Animation2D("resources/images/HeavyBandit_Spritesheet.png", ivec2(7,8));
    
    m_anim_params[STANDING] = {0,1,1};
    m_anim_params[RUNNING] = {8,8,8};
    m_anim_params[JUMPING] = {34,1,1};
    m_anim_params[FALLING] = {34,1,1};
    m_anim_params[IDLE] = {4,1,1};
}

Player::~Player() {
}

Player::AttackAnim* Player::get_attack_anim(Object* requester) {
    return requester && requester->getObjectType() & ITEM_TYPE ? &m_attack_anim : nullptr;
}

vec2 Player::getExtents() const {
    return PLAYER_EXTENTS;
}

float Player::get_attack_strength() {
    return are_flags_set(AGENT_STATE_ATTACKING) ? m_attack_anim.knockback : 0;
}

void Player::set_attacking(Object* requester) {
    if (requester && requester->getObjectType() & ITEM_TYPE) {
        set_flags(AGENT_STATE_ATTACKING);
    }
}

void Player::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y);
    body_def.fixedRotation = true;
    m_body = world->CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(PLAYER_HALF_DIMS.x, PLAYER_HALF_DIMS.y);

    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = PLAYER_DENSITY;
    fixture_def.friction = PLAYER_FRICTION;
    m_body->CreateFixture(&fixture_def);

    Agent::add_to_world(world, params);
}

// TODO: Make zip iterator class
void Player::render_inventory(SpriteBatch& batch, SpriteFont* font) const {
    if (m_inventory.is_empty()) return;
    if (!are_flags_set(PLAYER_STATE_SHOW_INVENTORY)) return;

    static const float ALPHA = 0.4;
    static const float BOUNDARY = 2;
    static const float TEXT_WIDTH = (ITEM_ICON_SIZE + BOUNDARY) * 3.f + BOUNDARY;
    static const vec2 DELTA(ITEM_ICON_SIZE + BOUNDARY, 0);

    vec2 pos(BOUNDARY, 100 - MEDIUM_TEXT_HEIGHT - BOUNDARY/2.f);
    const vector<Item*> items{m_inventory.prev(), m_inventory.curr(), m_inventory.next()};
    for (int i = 0; i < items.size(); i++) {
        items[i]->render_icon(batch, pos, i == 1 ? 1.0 : ALPHA);
        pos += DELTA;
    }
    font->drawText(batch, "Inventory", vec4(BOUNDARY/2.f, 100, TEXT_WIDTH, MEDIUM_TEXT_HEIGHT));
}

void Player::render_attack(SpriteBatch& batch) const {
    if (!are_flags_set(AGENT_STATE_ATTACKING)) return;

    auto offset = m_attack_anim.offset;
    offset.x = m_direction ? offset.x : PLAYER_DIMS.x - offset.x - m_attack_anim.dims.x;
    auto uv = m_direction ? m_attack_anim.anim.get_uv() : m_attack_anim.anim.get_flipped_uv();
    batch.addGlyph(vec4(getTopLeft() + offset, m_attack_anim.dims),
                   uv, m_attack_anim.anim.get_tex_id());
}

void Player::resolve_collision(const UpdateParams& params, b2ContactEdge* edge, b2Contact* contact, 
                       Object* obj) {
    static const float EPS = 1e-1;
    Agent::resolve_collision(params, edge, contact, obj);
    if (obj) {
        if (obj->getObjectType() & ITEM_TYPE) {
            Item* item = (Item*)obj;
            item->pickup(this, params.world);
            m_inventory.push_back(item);

            popup(PLAYER_STATE_SHOW_INVENTORY, m_inventory_event_id);
        } else if (obj->getObjectType() & ENEMY_TYPE) {
            Enemy* enemy = (Enemy*)obj;
            if (enemy->are_flags_set(AGENT_STATE_ATTACKING)) {
                // TODO: Vary damange
                applyDamage(0.5);
            } else if (!are_flags_set(AGENT_STATE_ATTACKING)) {
                applyDamage(0.25);
            }
        }
    }
}

void Player::handle_input(const UpdateParams& params) {
    if (InputManager::isPressed(SDLK_e) && !m_inventory.is_empty()) {
        // TODO: Clear event if button released
        if (InputManager::justPressed(SDLK_e)) {
            m_inventory.advance();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_e);}, HOLD_TIME);
        }
        popup(PLAYER_STATE_SHOW_INVENTORY, m_inventory_event_id);
    } else if (InputManager::isPressed(SDLK_q) && !m_inventory.is_empty()) {
        if (InputManager::justPressed(SDLK_q)) {
            m_inventory.retreat();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_q);}, HOLD_TIME);
        }
        popup(PLAYER_STATE_SHOW_INVENTORY, m_inventory_event_id);
    }

    // Should you have to be standing to move left and right?
    if (InputManager::isPressed(SDLK_d)) {
        applyForce(PLAYER_FORCE, 0);
    } else if (InputManager::isPressed(SDLK_a)) {
        applyForce(-PLAYER_FORCE, 0);
    }
    if (OBJECT_ON_GROUND(m_motion_state)) {
        if (InputManager::isPressed(SDLK_w)) {
            applyForce(0, PLAYER_JUMP_FORCE);
        }
    }
    if (InputManager::justPressed(SDLK_SPACE) && !m_inventory.is_empty()) {
        CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_SPACE);}, HOLD_TIME);
        m_inventory.curr()->use(params);
    }
}

void Player::update(const UpdateParams& params) {
    Agent::update(params);
    handle_input(params);

    if (are_flags_set(AGENT_STATE_ATTACKING)) {
        m_attack_anim.anim.step(params.dt*m_attack_anim.speed);
        if (m_attack_anim.anim.get_time() > m_attack_anim.anim.get_length()*m_attack_anim.num_cycles) {
            unset_flags(AGENT_STATE_ATTACKING);
        }
    }
}