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
    batch.addGlyph(vec4(m_top_left, 2.f*m_extents), vec4(0,0,1,1), m_tex.id,
                    m_color, m_angle);
}


AnimationComponent::AnimationComponent(crstring texture, crivec2 anim_dims, 
                                       MotionAnimation anims[], crvec4 color) : 
    ObjectGraphicsComponent(texture, color, COMPONENT_ANIMATION_LIST_ID) {
    m_anim = Animation2D(m_tex_file, anim_dims);
    if (anim_dims != ivec2(1)) {
        memcpy(m_anim_params, anims, NUM_MOTION_STATES*sizeof(MotionAnimation));
    }
}

void AnimationComponent::render(SpriteBatch& batch) const {
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

vec2 PlanetGraphicsComponent::getOffset() const {
    return vec2(-(*m_tiles)[1].size()*TILE_SIZE/2., m_sea_level*TILE_SIZE);   
}

void PlanetGraphicsComponent::render(nta::SpriteBatch& batch) const {
    const vec2 offset = getOffset();
    for (int r = 0; r < m_tiles->size(); r++) {
        for (int c = 0; c < (*m_tiles)[1].size(); c++) {
            (*m_tiles)[r][c].render(batch, offset - (float)r*TILE_DY + (float)c*TILE_DX);
        }
    }
}

void PlanetGraphicsComponent::render_debug(nta::DebugBatch& dbatch) const {
    static const size_t NUM_PIECES = 150;
    const vec2 offset = getOffset();

    int num_rows = m_tiles->size();
    int num_cols = (*m_tiles)[1].size();

    for (int r = 0; r <= num_rows; r++) {
        vec4 color = r == m_sea_level ? vec4(0,1,1,1) : vec4(1);
        vec2 left(offset - (float)r*TILE_DY), 
             right(offset - (float)r*TILE_DY + (float)num_cols*TILE_DX);
        dbatch.addLine(left, right, NUM_PIECES, color);
    }
    for (int c = 0; c <= num_cols; c++) {
        vec2 top(offset + (float)c*TILE_DX),
             bottom(offset - (float)num_rows*TILE_DY + (float)c*TILE_DX);
        dbatch.addLine(top, bottom, NUM_PIECES);
    }
}

void PlanetGraphicsComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_TILES) {
        m_tiles = (vector<vector<Tile>>*)msg.data;
    } else if (msg == MESSAGE_RECEIVE_SL) {
        m_sea_level = *(int*)msg.data;
    } else {
        GraphicsComponent::receive(msg);
    }
}