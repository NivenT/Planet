#include <nta/ResourceManager.h>

#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void GraphicsComponent::receive(const Message& msg) {
    if (msg == MESSAGE_TOGGLE_VISIBILITY) {
        m_invisible = !m_invisible;
    }
}

void ObjectGraphicsComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_TL) {
        m_top_left = *(vec2*)msg.data;
    } else if (msg == MESSAGE_RECEIVE_EXT) {
        m_extents = *(vec2*)msg.data;
    } else if (msg == MESSAGE_RECEIVE_ANG) {
        m_angle = *(float*)msg.data;
    } else {
        GraphicsComponent::receive(msg);
    }
}

TextureComponent::TextureComponent(crstring texture, crvec4 color) : 
    ObjectGraphicsComponent(texture, color) {
    m_tex = ResourceManager::getTexture(m_tex_file).get_data_or(GLTexture());
}

void TextureComponent::render(SpriteBatch& batch) const {
    if (m_invisible) return;
    batch.addGlyph(vec4(m_top_left, 2.f*m_extents), vec4(0,0,1,1), m_tex.id,
                    m_color, m_angle);
}

void TextureComponent::render_icon(SpriteBatch& batch, crvec2 top_left, 
                                   float alpha) const {
    batch.addGlyph(vec4(top_left, ITEM_ICON_DIMS), vec4(0,0,1,1), m_tex.id,
                   vec4(m_color.r, m_color.g, m_color.b, m_color.a * alpha));
}

AnimationComponent::AnimationComponent(crstring texture, crivec2 anim_dims, 
                                       const MotionAnimation anims[], crvec4 color) : 
    ObjectGraphicsComponent(texture, color) {
    m_anim = Animation2D(m_tex_file, anim_dims);
    if (anim_dims != ivec2(1)) {
        memcpy(m_anim_params, anims, NUM_MOTION_STATES*sizeof(MotionAnimation));
    }
}

void AnimationComponent::render(SpriteBatch& batch) const {
    if (m_invisible) return;
    const vec4 uv = m_flipped ? m_anim.get_flipped_uv() : m_anim.get_uv();
    batch.addGlyph(vec4(m_top_left, 2.f*m_extents), uv, m_anim.get_tex_id(),
                    m_color, m_angle);
}

void AnimationComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_DIR) {
        m_flipped = !*(bool*)msg.data;
    } else if (msg == MESSAGE_RECEIVE_MOTION_STATE) {
        ObjectMotionState state = *(ObjectMotionState*)msg.data;
        if (state != m_motion_state) {
            auto p = m_anim_params[state];
            m_anim.switch_animation(p.start, p.length, p.speed);
        }
        m_motion_state = state;
    } else {
        ObjectGraphicsComponent::receive(msg);
    }
}

void AnimationComponent::step(float dt) {
    m_anim.step(dt);
}

void PlayerAnimationComponent::step(float dt) {
    if (m_motion_state == RUNNING) {
        m_anim.set_speed(m_anim_params[RUNNING].speed*m_vel_x/PLAYER_MAX_SPEED.x);
    }
    AnimationComponent::step(dt);
}

void PlayerAnimationComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_VEL) {
        m_vel_x = *(float*)msg.data;
    } else {
        AnimationComponent::receive(msg);
    }
}
