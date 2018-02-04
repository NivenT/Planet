#include <nta/InputManager.h>

#include "Player.h"

using namespace std;
using namespace glm;
using namespace nta;

Player::Player() : Agent(PLAYER_COLOR, PLAYER_INIT_HEALTH) {
}

Player::~Player() {
}

void Player::add_to_world(b2World* world, const CreationParams& _) {
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
}

void Player::render(SpriteBatch& batch) const {
    Light light(getCenter(), PLAYER_COLOR, PLAYER_HALF_DIMS.x*1.618, m_health/PLAYER_INIT_HEALTH);
    light.render(batch);
}

void Player::update(const UpdateParams& params) {
    static const float EPS = 1e-1;
    Agent::update(params);

    if (InputManager::isPressed(SDLK_d)) {
        m_body->ApplyForceToCenter(b2Vec2(PLAYER_FORCE, 0), true);
    } else if (InputManager::isPressed(SDLK_a)) {
        m_body->ApplyForceToCenter(b2Vec2(-PLAYER_FORCE, 0), true);
    }

    bool is_standing = false;
    for (b2ContactEdge* edge = m_body->GetContactList(); edge != nullptr; edge = edge->next) {
        if (edge->contact->IsTouching()) {
            b2WorldManifold manifold;
            edge->contact->GetWorldManifold(&manifold);

            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < getCenter().y - PLAYER_HALF_DIMS.y + EPS) {
                    is_standing = true;
                    // No matter what you think of this, it's better than break + if
                    goto contact_check_over;
                }
            }
        }
    }
    contact_check_over:

    if (is_standing && InputManager::isPressed(SDLK_w)) {
        m_body->ApplyForceToCenter(b2Vec2(0, PLAYER_JUMP_FORCE), true);
    }
}