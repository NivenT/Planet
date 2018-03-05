#include <nta/ResourceManager.h>

#include "Enemy.h"

using namespace nta;
using namespace glm;

Enemy::Enemy(crstring texture, float health, crvec4 color, uint16_t type) : 
    Agent(color, health, type | ENEMY_TYPE), m_extents(0) {
    m_tex = ResourceManager::getTexture(texture);
}

Enemy::~Enemy() {
}

void Enemy::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    m_body = world->CreateBody(&body_def);

    m_extents = params.extents;
    b2PolygonShape body_shape;
    body_shape.SetAsBox(m_extents.x, m_extents.y);

    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    m_body->CreateFixture(&fixture_def);

    Agent::add_to_world(world, params);
}

void Enemy::render(SpriteBatch& batch) const {
    const vec2 center = getCenter();
    batch.addGlyph(vec4(center.x - m_extents.x, center.y + m_extents.y, 2.f*m_extents),
                        vec4(0,0,1,1), m_tex.id, m_color);
}
