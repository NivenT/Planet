#include <nta/CallbackManager.h>
#include <nta/ResourceManager.h>

#include "Agent.h"

using namespace std;
using namespace glm;
using namespace nta;

Agent::Agent(crvec2 s, crvec4 c, float h, uint16_t type) : Object(s, c, type | AGENT_TYPE), m_health(h),
    m_state_flags(0), m_health_event_id(0), m_max_health(h) {
}

Agent::~Agent() {
    m_health = 0;
    m_state_flags = 0;
}

vec2 Agent::getDirection() const {
    return normalize(getVelocity());
}

float Agent::getHealth() const {
    return m_health;
}

bool Agent::applyDamage(float damage) {
    m_health -= damage;
    popup(AGENT_STATE_SHOW_HEALTH, m_health_event_id);
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

void Agent::popup(int flags, uint64_t& eid, int when) {
    if (are_flags_set(flags)) {
        CallbackManager::delay(eid, when);
    } else {
        eid = set_flags_until(flags, when);
    }
}

void Agent::render(SpriteBatch& batch) const {
    const vec4 uv = m_direction == m_natural_direction ? m_anim.get_uv() : 
                                                         m_anim.get_flipped_uv();
    batch.addGlyph(vec4(getTopLeft(), 2.f*getExtents()), uv, m_anim.get_tex_id(),
                    m_color, getOrientation());
    render_health(batch);
}

void Agent::render_health(SpriteBatch& batch) const {
    if (m_health <= 0) return;
    if (!are_flags_set(AGENT_STATE_SHOW_HEALTH)) return;

    static const float ALPHA = 0.4;
    static const float PERCENT_WIDTH = 0.9;
    static const float PERCENT_ABOVE_TOP = 0.1;
    static const float HEIGHT = PLAYER_DIMS.y/10.f;
    // unsafe but meh
    static const GLuint tex = ResourceManager::getTexture("resources/images/health.png").get_data().id;

    const vec2 extents = getExtents();
    const float width = PERCENT_WIDTH*extents.x*2.f;
    const vec2 bl = getTopLeft() + vec2((1.-PERCENT_WIDTH), PERCENT_ABOVE_TOP)*getExtents();

    const vec3 c = getObjectType() & PLAYER_TYPE ? PLAYER_HEATLH_COLOR :
                   getObjectType() & ENEMY_TYPE  ? ENEMY_HEALTH_COLOR : DEFAULT_HEALTH_COLOR;
    batch.addGlyph(vec4(bl.x, bl.y + HEIGHT, width, HEIGHT), vec4(0, 0, 1, 1), tex,
                   vec4(c.r, c.g, c.b, ALPHA));
    batch.addGlyph(vec4(bl.x, bl.y + HEIGHT, width*m_health/m_max_health, HEIGHT), vec4(0, 0, 1, 1), 
                   tex, vec4(c.r, c.g, c.b, 1));
}

void Agent::update(const UpdateParams& params) {
    Object::update(params);

    /*
    if (m_motion_state == RUNNING) {
        m_anim.set_speed(m_anim_params[RUNNING].speed*getVelocity().x/getMaxSpeed().x);
    }
    */
    m_anim.step(params.dt);
    if (m_motion_state != m_prev_motion_state) {
        auto p = m_anim_params[m_motion_state];
        m_anim.switch_animation(p.start, p.length, p.speed);
    }
}