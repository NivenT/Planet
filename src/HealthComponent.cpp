#include <nta/ResourceManager.h>

#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void HealthComponent::render(SpriteBatch& batch) const {
    if (m_health <= 0 || !m_show_bar) return;

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
    if (msg == MESSAGE_RECEIVE_TL) {
        m_top_left = *(vec2*)msg.data;
    } else if (msg == MESSAGE_RECEIVE_EXT) {
        m_extents = *(vec2*)msg.data;
    } else if (msg == MESSAGE_TOGGLE_SHOW_HEALTH_BAR) {
        m_show_bar = !m_show_bar;
    }
}