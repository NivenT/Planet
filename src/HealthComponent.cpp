#include <nta/ResourceManager.h>

#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void HealthComponent::render(SpriteBatch& batch) const {
    if (m_health <= 0 || !m_flag) return;

    static const float ALPHA = 0.4;
    static const float PERCENT_WIDTH = 0.9;
    static const float PERCENT_ABOVE_TOP = 0.1;
    static const float HEIGHT = PLAYER_DIMS.y/10.f;
    // unsafe but meh
    static const GLuint tex = ResourceManager::getTexture("resources/images/health.png").get_data().id;

    const float width = PERCENT_WIDTH*m_extents.x*2.f;
    const vec2 bl = m_top_left + vec2((1.-PERCENT_WIDTH), PERCENT_ABOVE_TOP)*m_extents;

    batch.addGlyph(vec4(bl.x, bl.y + HEIGHT, width, HEIGHT), vec4(0, 0, 1, 1), tex,
                   vec4(m_bar_color, ALPHA));
    batch.addGlyph(vec4(bl.x, bl.y + HEIGHT, width*m_health/m_max_health, HEIGHT), vec4(0, 0, 1, 1), 
                   tex, vec4(m_bar_color, 1));
}

void HealthComponent::receive(const nta::Message& msg) {
    switch(msg.type) {
        case MESSAGE_RECEIVE_TL: m_top_left = *(vec2*)msg.data; break;
        case MESSAGE_RECEIVE_EXT: m_extents = *(vec2*)msg.data; break;
        case MESSAGE_RECEIVE_VEL: m_vel = *(vec2*)msg.data; break;
        case MESSAGE_RECEIVE_MASS: m_mass = *(float*)msg.data; break;
        case MESSAGE_TOGGLE_SHOW_HEALTH_BAR: m_flag = !m_flag; break;
        case MESSAGE_RESOLVE_COLLISION: {
            CollisionParams p = *(CollisionParams*)msg.data;
            auto health = m_system->get_component<HealthComponent>(p.other);
            if (health.is_some()) {
                if (!(m_mask_bits & health.unwrap().m_category_bits)) return;

                float mom = m_mass*m_vel.length();
                float other_mom = health.unwrap().m_mass*health.unwrap().m_vel.length();
                float scale = health.unwrap().m_mass*(mom + other_mom)/(m_mass + health.unwrap().m_mass);

                m_health -= scale*HEALTH_DEPLETION_RATE;
                start_countdown();

                if (m_health <= 0) {
                    Entity me = m_system->get_owner(this);
                    m_system->broadcast<GarbageComponent>(Message(MESSAGE_DESTROY_ENTITY, &me));
                }
            }
        } break;
    }
}