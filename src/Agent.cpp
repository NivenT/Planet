#include <nta/CallbackManager.h>

#include "Agent.h"

using namespace std;
using namespace glm;
using namespace nta;

Agent::Agent(glm::vec4 c, float m, uint16_t type) : Object(c, type | AGENT_TYPE), m_health(m),
    m_state_flags(0) {
}

Agent::~Agent() {
    m_health = 0;
    m_state_flags = 0;
}

vec2 Agent::getDirection() const {
    return normalize(getVelocity());
}

vec2 Agent::getVelocity() const {
    return vec2(m_body->GetLinearVelocity().x, m_body->GetLinearVelocity().y);
}

float Agent::getHealth() const {
    return m_health;
}

bool Agent::applyDamage(float damage) {
    m_health -= damage;
    return m_health <= 0;
}

bool Agent::are_flags_set(AgentFlags flags) const {
    return (flags & m_state_flags) == flags;
}

void Agent::set_flags(AgentFlags flags) {
    m_state_flags |= flags;
}

uint64_t Agent::set_flags_until(AgentFlags flags, uint64_t when) {
    set_flags(flags);
    return CallbackManager::setTimeout([this, flags]{unset_flags(flags);}, when);
}

void Agent::unset_flags(AgentFlags flags) {
    m_state_flags &= ~flags;
}

uint64_t Agent::unset_flags_until(AgentFlags flags, uint64_t when) {
    unset_flags(flags);
    return CallbackManager::setTimeout([this, flags]{set_flags(flags);}, when);
}