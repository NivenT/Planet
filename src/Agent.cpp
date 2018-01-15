#include "Agent.h"

using namespace std;
using namespace glm;
using namespace nta;

Agent::Agent(glm::vec4 c, float m) : Object(c), m_health(m) {
}

Agent::~Agent() {
    m_health = 0;
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