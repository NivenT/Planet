#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void EventQueueComponent::process() {
    while (!m_spawns.empty()) {
        auto& spawn = m_spawns.top();

        /// \todo Don't duplicate code
        Entity id = m_system->gen_entity();

        m_system->add_component<AnimationComponent>(id, spawn.tex, spawn.anim_dims,
                                                spawn.anims, spawn.color);
        m_system->add_component<PhysicsComponent>(id, spawn.max_speed);
        m_system->add_component<HealthComponent>(id, spawn.init_health, ENEMY_HEALTH_CATEGORY,
                                             ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR);
        m_system->add_component<ScriptComponent>(id, spawn.update_script);

        m_system->add_component_to_list<GraphicsComponent>(&m_system->get_component<AnimationComponent>(id).unwrap());
        m_system->add_component_to_list<ControllerComponent>(&m_system->get_component<ScriptComponent>(id).unwrap());

        PhysicsComponent& physics = m_system->get_component<PhysicsComponent>(id).unwrap();
        physics.add_to_world(m_world, spawn, id);

        vec2 tl = physics.getTopLeft(), cen = physics.getCenter();
        float ang = physics.getOrientation();
        float mass = physics.getMass();
        m_system->broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_system->broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_system->broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
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