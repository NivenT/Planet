#include <nta/ResourceManager.h>

#include "Enemy.h"
#include "ChaiManager.h"

using namespace nta;
using namespace glm;

Enemy::Enemy(crstring texture, crstring update, float health, float speed, crvec4 color, uint16_t type) : 
    Agent(color, health, type | ENEMY_TYPE), m_extents(0), m_update_script(update), m_max_speed(speed) {
    m_tex = ResourceManager::getTexture(texture);
}

Enemy::~Enemy() {
}

vec2 Enemy::getExtents() const {
    return m_extents;
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
    fixture_def.restitution = params.restitution;
    m_body->CreateFixture(&fixture_def);

    Agent::add_to_world(world, params);
}

void Enemy::render(SpriteBatch& batch) const {
    const vec2 center = getCenter();
    batch.addGlyph(vec4(center.x - m_extents.x, center.y + m_extents.y, 2.f*m_extents),
                        vec4(0,0,1,1), m_tex.id, m_color, getOrientation());
    render_health(batch);
}

void Enemy::resolve_collision(const UpdateParams& params, b2ContactEdge* edge, b2Contact* contact, 
                              Object* obj) {
    static const float EPS = 1e-1;
    Agent::resolve_collision(params, edge, contact, obj);
    if (obj) {
        if (obj->getObjectType() & PLAYER_TYPE) {
            // TODO: Vary damange
            applyDamage(0.5);
        }
    }
}

void Enemy::update(const UpdateParams& params) {
    Agent::update(params);
    if (m_update_script != "") ChaiManager::eval_script(m_update_script, chaiscript::var(this));
}
