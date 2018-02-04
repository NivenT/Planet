#include <nta/ResourceManager.h>

#include "Item.h"

using namespace nta;
using namespace glm;

Item::Item(crvec4 c, crstring texture, crvec2 extents) : Object(c), m_owner(nullptr), 
    m_extents(extents) {
    m_tex = ResourceManager::getTexture(texture);
}

Item::~Item() {
    m_owner = nullptr;
}

void Item::add_to_world(b2World* world, const CreationParams& params) {
    b2BodyDef body_def;
    //body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    m_body = world->CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(m_extents.x, m_extents.y);

    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    m_body->CreateFixture(&fixture_def); 
}

void Item::render(SpriteBatch& batch) const {
    /*
    // Assume single quad for fixture
    b2PolygonShape* quad = (b2PolygonShape*)m_body->GetFixtureList()->GetShape();
    vec2 v1(quad->m_vertices[0].x, quad->m_vertices[0].y), 
         v2(quad->m_vertices[2].x, quad->m_vertices[2].y);
    */
    batch.addGlyph(getCenter()-m_extents, getCenter()+m_extents, vec4(0,0,1,1), m_tex.id, m_color);
}

void Item::update(const UpdateParams& params) {
    Object::update(params);
}