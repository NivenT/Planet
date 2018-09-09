#include "Spawner.h"

using namespace std;
using namespace glm;
using namespace nta::utils;

// ugh formatting
Spawner::Spawner(nta::crstring texture, EnemyParams spawn, float health, nta::crvec4 color, 
                 uint16_t type) : 
    Enemy(texture, "", health, SPAWNER_MAX_SPEED, color,  type | SPAWNER_TYPE), 
    m_spawn(spawn), m_spawn_rate(SPAWNER_SLOW_RATE), m_time(0) {   
}

Spawner::Spawner(const SpawnerParams& params) : Enemy(params) {
    m_spawn = EnemyParams::load(Json::from_file(params.spawn));
    set_spawn_rate(params.spawn_rate);
}
void Spawner::set_spawn_rate(float rate) {
    m_spawn_rate = abs(rate);
}

void Spawner::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    m_body = world->CreateBody(&body_def);

    m_extents = params.extents;
    b2PolygonShape body_shape;
    body_shape.SetAsBox(m_extents.x, m_extents.y);

    // Solid fixture for certain collisions (e.g. with ground)
    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    fixture_def.restitution = params.restitution;
    fixture_def.filter.maskBits = SPAWNER_MASK_BITS;
    m_body->CreateFixture(&fixture_def);

    // Sensor fixture for detecting collisions w/o resolution (e.g. with player)
    b2FixtureDef sensor_def;
    sensor_def.shape = &body_shape;
    sensor_def.isSensor = true;
    m_body->CreateFixture(&sensor_def);

    Agent::add_to_world(world, params);
}

void Spawner::update(const UpdateParams& params) {
    Enemy::update(params);
    while (m_time > m_spawn_rate) {
        m_time -= m_spawn_rate;

        //m_spawn.planet = params.planet;
        m_spawn.position = getCenter();

        /// \todo pass m_params to notify instead of calling add_to_world yourself
        notify(nta::Message(MESSAGE_SPAWN_ENEMY, (void*)&m_spawn));
    }

    m_time += params.dt;
}