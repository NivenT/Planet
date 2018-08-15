#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <nta/Animation2D.h>

#include "Object.h"

typedef uint16_t AgentFlags;

class Agent : public Object {
protected:
    void set_flags(AgentFlags flags);
    /// Returns event id
    uint64_t set_flags_until(AgentFlags flags, uint64_t when);
    void unset_flags(AgentFlags flags);
    uint64_t unset_flags_until(AgentFlags flags, uint64_t when);
    //void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);

    float m_health;
    const float m_max_health;
    uint64_t m_health_event_id;
    AgentFlags m_state_flags;
    nta::Animation2D m_anim;
    MotionAnimation m_anim_params[OBJECT_NUM_MOTION_STATES];
public:
    Agent(crvec2 speed, crvec4 color, float health, uint16_t type);
    virtual ~Agent();
    virtual glm::vec2 getDirection() const;
    float getHealth() const;
    bool applyDamage(float damage);
    bool are_flags_set(AgentFlags flags) const;
    // TODO: Think of better name
    void popup(int flag, uint64_t& eid, int when = STANDARD_POPUP_TIME);
    virtual void render(nta::SpriteBatch& batch) const;
    virtual void render_health(nta::SpriteBatch& batch) const;
    virtual void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // AGENT_H_INCLUDED