#include <nta/ResourceManager.h>

#include "Enemy.h"
#include "Player.h"
#include "ChaiManager.h"

using namespace nta;
using namespace glm;

Enemy::Enemy(crstring texture, crstring update, float health, crvec2 speed, crvec4 color, uint16_t type) : 
    Agent(speed, color, health, type | ENEMY_TYPE), m_extents(0), m_update_script(update) {
    // unsafe but meh
    m_anim = Animation2D(texture, ivec2(1,1));
}

Enemy::Enemy(const EnemyParams& p, uint16_t type) :
    Enemy(p.tex, p.update_script, p.init_health, p.max_speed, p.color, type) {
    memcpy(m_anim_params, p.anims, OBJECT_NUM_MOTION_STATES*sizeof(MotionAnimation));
    m_anim = Animation2D(p.tex, p.anim_dims);
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

void Enemy::resolve_collision(const UpdateParams& params, b2ContactEdge* edge, b2Contact* contact, 
                              Object* obj) {
    static const float EPS = 1e-1;
    Agent::resolve_collision(params, edge, contact, obj);
    if (obj) {
        if (obj->getObjectType() & PLAYER_TYPE) {
            Player* player = (Player*)obj;
            if (player->are_flags_set(AGENT_STATE_ATTACKING)) {
                applyDamage(1.2);

                vec2 force = player->get_attack_strength()*normalize(getCenter()-player->getCenter());
                applyForce(force.x, force.y);
            } else if (!are_flags_set(AGENT_STATE_ATTACKING)) {
                applyDamage(0.5);
            }
        }
    }
}

void Enemy::update(const UpdateParams& params) {
    Agent::update(params);
    if (m_update_script != "") {
        ChaiManager::eval_script(m_update_script, chaiscript::var(this), chaiscript::var(params));
    }
}
