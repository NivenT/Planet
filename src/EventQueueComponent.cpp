#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void EventQueueComponent::process() {
    while (!m_spawns.empty()) {
        auto& spawn = m_spawns.top();

        /// \todo Don't duplicate code
        EntityID id = m_system->gen_entity();

        m_system->add_component(new AnimationComponent(spawn.tex, spawn.anim_dims,
                                                       spawn.anims, spawn.color),
                                id);

        auto physics = new PhysicsComponent(spawn.max_speed);
        m_system->add_component(physics, id);
        physics->add_to_world(m_world, spawn, id);

        m_system->add_component(new HealthComponent(spawn.init_health, ENEMY_HEALTH_CATEGORY,
                                                    ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR), 
                                id);
        m_system->add_component(new ScriptComponent(spawn.update_script), id);

        float mass = physics->getMass();
        m_system->broadcast(Message(MESSAGE_RECEIVE_EXT, &spawn.extents), id);
        m_system->broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);

        m_spawns.pop();
    }
}

void EventQueueComponent::receive(const nta::Message& msg) {
    switch(msg.type) {
        case MESSAGE_SPAWN_ENEMY: m_spawns.push(*(EnemyParams*)msg.data); break;
    }
}