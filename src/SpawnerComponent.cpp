#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

SpawnerComponent::SpawnerComponent(int st, const EnemyParams& e) : m_spawn_time(st),
    m_spawn(e) {
    start_countdown(m_spawn_time);
}

void SpawnerComponent::onFinish() {
    m_spawn.position = m_pos;
    m_system->broadcast<EventQueueComponent>(Message(MESSAGE_SPAWN_ENEMY, &m_spawn));
    start_countdown(m_spawn_time);
}

void SpawnerComponent::receive(const Message& msg) {
    switch(msg.type) {
        case MESSAGE_RECEIVE_CEN: m_pos = *(vec2*)msg.data; break;
    }
}