#include <nta/ResourceManager.h>

#include "Item.h"
#include "ChaiManager.h"

using namespace nta;
using namespace glm;

Item::Item(crvec4 c, crstring texture, crvec2 extents, crstring use, crvec2 speed, uint16_t type) : 
    Object(speed, c, type | ITEM_TYPE), m_owner(nullptr), m_extents(extents), m_equipped(false),
    m_use_script(use) {
    m_tex = ResourceManager::getTexture(texture);
}

Item::Item(crstring texture, crvec2 extents, crstring use, crvec2 speed, uint16_t type) : 
    Item(vec4(1), texture, extents, use, speed, type) {
}

Item::~Item() {
    m_owner = nullptr;
}

vec2 Item::getExtents() const {
    return m_extents;
}

void Item::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    m_body = world->CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(m_extents.x, m_extents.y);

    // Solid fixture for certain collisions (e.g. with ground)
    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    fixture_def.filter.maskBits = ITEM_MASK_BITS;
    m_body->CreateFixture(&fixture_def);

    // Sensor fixture for detecting collisions w/o resolution (e.g. with player)
    b2FixtureDef sensor_def;
    sensor_def.shape = &body_shape;
    sensor_def.isSensor = true;
    m_body->CreateFixture(&sensor_def);

    m_planet = params.planet;
    m_equipped = false; // Should be unnecessary but can't hurt
    Object::add_to_world(world, params);
}

void Item::render(SpriteBatch& batch) const {
    if (!m_equipped) {
        const vec2 center = getCenter();
        batch.addGlyph(vec4(center.x - m_extents.x, center.y + m_extents.y, 2.f*m_extents),
                            vec4(0,0,1,1), m_tex.id, m_color, getOrientation());
    }
}

void Item::render_icon(SpriteBatch& batch, crvec2 top_left, float transparency) const {
    batch.addGlyph(vec4(top_left, ITEM_ICON_DIMS), vec4(0,0,1,1), m_tex.id,
                   vec4(m_color.r, m_color.g, m_color.b, m_color.a * transparency));
}

void Item::use(const UpdateParams& params) {
    if (m_use_script != "") {
        ChaiManager::eval_script(m_use_script, chaiscript::var(this), chaiscript::var(params));
    }
}

void Item::pickup(Agent* owner, b2World* world) {
    m_equipped = true;
    m_owner = owner;

    world->DestroyBody(m_body);
}