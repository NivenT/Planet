#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include "Object.h"

typedef uint16_t AgentFlags;

class Agent : public Object {
protected:
    void set_flags(AgentFlags flags);
    void set_flags_until(AgentFlags flags, uint64_t when);
    void unset_flags(AgentFlags flags);
    void unset_flags_until(AgentFlags flags, uint64_t when);

    float m_health;
    AgentFlags m_state_flags;
public:
    Agent(glm::vec4 c, float m, uint16_t type);
    virtual ~Agent();
    virtual glm::vec2 getDirection() const;
    glm::vec2 getVelocity() const;
    float getHealth() const;
    bool applyDamage(float damage);
    bool are_flags_set(AgentFlags flags) const;
};

#endif // AGENT_H_INCLUDED