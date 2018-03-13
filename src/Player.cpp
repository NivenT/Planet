#include <nta/InputManager.h>
#include <nta/CallbackManager.h>

#include "Player.h"

using namespace std;
using namespace glm;
using namespace nta;

Player::Player(uint16_t type) : Agent(PLAYER_COLOR, PLAYER_INIT_HEALTH, type | PLAYER_TYPE),
    m_inventory_event_id(0) {
}

Player::~Player() {
}

void Player::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y);
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

void Player::render(SpriteBatch& batch) const {
    Light light(getCenter(), m_color, PLAYER_HALF_DIMS.x*1.618, m_health/PLAYER_INIT_HEALTH);
    light.render(batch);
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

void Player::handle_collisions(const UpdateParams& params) {
    static const float EPS = 1e-1;

    m_is_standing = false;
    for (b2ContactEdge* edge = m_body->GetContactList(); edge != nullptr; edge = edge->next) {
        void* object = edge->other->GetUserData();
        if (object) {
            const uint16_t type = ((Object*)object)->getObjectType();
            if (type & ITEM_TYPE) {
                Item* item = (Item*)object;
                item->pickup(this, params.world);
                m_inventory.push_back(item);

                set_flags_until(PLAYER_STATE_SHOW_INVENTORY, STANDARD_POPUP_TIME);
            }
        }

        b2Contact* contact = edge->contact;
        if (contact->IsTouching() && 
                !(contact->GetFixtureA()->IsSensor() || contact->GetFixtureB()->IsSensor())) {
            b2WorldManifold manifold;
            contact->GetWorldManifold(&manifold);

            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < getCenter().y - PLAYER_HALF_DIMS.y + EPS) {
                    m_is_standing = true;
                }
            }
        }
    }
}

void Player::popup_inventory() {
    if (are_flags_set(PLAYER_STATE_SHOW_INVENTORY)) {
        CallbackManager::delay(m_inventory_event_id, STANDARD_POPUP_TIME);
    } else {
        m_inventory_event_id = set_flags_until(PLAYER_STATE_SHOW_INVENTORY, STANDARD_POPUP_TIME);
    }
}

void Player::update(const UpdateParams& params) {
    Agent::update(params);
    handle_collisions(params);

    if (InputManager::isPressed(SDLK_e) && !m_inventory.is_empty()) {
        // TODO: Clear event if button released
        if (InputManager::justPressed(SDLK_e)) {
            m_inventory.advance();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_e);}, HOLD_TIME);
        }
        popup_inventory();
    } else if (InputManager::isPressed(SDLK_q) && !m_inventory.is_empty()) {
        if (InputManager::justPressed(SDLK_q)) {
            m_inventory.retreat();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_q);}, HOLD_TIME);
        }
        popup_inventory();
    }

    if (InputManager::isPressed(SDLK_d)) {
            m_body->ApplyForceToCenter(b2Vec2(PLAYER_FORCE, 0), true);
    } else if (InputManager::isPressed(SDLK_a)) {
        m_body->ApplyForceToCenter(b2Vec2(-PLAYER_FORCE, 0), true);
    }
    if (m_is_standing) {
        if (InputManager::isPressed(SDLK_w)) {
            m_body->ApplyForceToCenter(b2Vec2(0, PLAYER_JUMP_FORCE), true);
        }
    }
}