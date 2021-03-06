#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;
using namespace nta::utils;

void AttackComponent::render(SpriteBatch& batch) const {
    if (!is_attacking()) return;

    auto offset = m_offset;
    if (!m_direction) {
        offset.x = -m_extents.x*2.f-m_offset.x-m_dims.x;
    }
    auto uv = m_direction ? m_anim.get_uv() : m_anim.get_flipped_uv();
    auto tr = m_top_left + vec2(m_extents.x*2.f, 0);
    batch.addGlyph(vec4(tr + offset, m_dims), uv, m_anim.get_tex_id());
}

void AttackComponent::step(float dt) {
    if (!is_attacking()) return;

    m_anim.step(dt);
    if (m_anim.get_time() > m_anim.get_length()*m_num_cycles) {
        m_num_cycles = 0;
    }
}

void AttackComponent::receive(const Message& msg) {
    switch(msg.type) {
        case MESSAGE_RECEIVE_TL: m_top_left = *(vec2*)msg.data; break;
        case MESSAGE_RECEIVE_EXT: m_extents = *(vec2*)msg.data; break;
        case MESSAGE_RECEIVE_DIR: m_direction = *(bool*)msg.data; break;
        case MESSAGE_RESOLVE_COLLISION: {
            if (!is_attacking()) return;
            
            CollisionParams p = *(CollisionParams*)msg.data;
            Option<PhysicsComponent&> physics = m_system->get_component<PhysicsComponent>(p.other);
            Option<HealthComponent&> health = m_system->get_component<HealthComponent>(p.other);
            if (physics.is_some() && health.is_some()) {
                // should probably use center instead of top left but meh
                vec2 force = m_knockback*normalize(physics.unwrap().getTopLeft()-m_top_left);
                physics.unwrap().applyForce(force.x, force.y);
            }
        } break;
    }
}
