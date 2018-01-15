#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include "Object.h"

class Agent : public Object {
protected:
    float m_health;
public:
    Agent(glm::vec4 c, float m);
    virtual ~Agent();
    virtual glm::vec2 getDirection() const;
    glm::vec2 getVelocity() const;
    float getHealth() const;
    bool applyDamage(float damage);
};

#endif // AGENT_H_INCLUDED