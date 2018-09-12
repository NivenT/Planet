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

    /*
    const vec3 c = getObjectType() & PLAYER_TYPE ? PLAYER_HEATLH_COLOR :
                   getObjectType() & ENEMY_TYPE  ? ENEMY_HEALTH_COLOR : DEFAULT_HEALTH_COLOR;
    */
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
            HealthComponent* health = (HealthComponent*)m_system->get_component(p.other, COMPONENT_HEALTH_LIST_ID);
            if (health) {
                if (!(m_mask_bits & health->m_category_bits)) return;

                float mom = m_mass*m_vel.length();
                float other_mom = health->m_mass*health->m_vel.length();
                float scale = health->m_mass*(mom + other_mom)/(m_mass + health->m_mass);

                m_health -= scale*HEALTH_DEPLETION_RATE;
                start_countdown();

                if (m_health <= 0) {
                    EntityID me = m_system->get_owner(this);
                    m_system->broadcast(Message(MESSAGE_DESTROY_ENTITY, &me),
                                        COMPONENT_GARBAGE_LIST_ID);
                }
            }
        } break;
    }
}