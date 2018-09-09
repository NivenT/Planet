#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

SpawnerComponent::SpawnerComponent(int st, const EnemyParams& e) : m_spawn_time(st),
    m_spawn(e), CountdownComponent(0) {
    start_countdown(m_spawn_time);
}

void SpawnerComponent::onFinish() {
    m_spawn.position = m_pos;
    m_system->broadcast(Message(MESSAGE_SPAWN_ENEMY, &m_spawn), COMPONENT_EVENTQ_LIST_ID);
    start_countdown(m_spawn_time);
}

void SpawnerComponent::receive(const Message& msg) {
    switch(msg.type) {
        case MESSAGE_RECEIVE_CEN: m_pos = *(vec2*)msg.data; break;
    }
}