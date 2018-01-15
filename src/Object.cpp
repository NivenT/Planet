#include "Object.h"

using namespace std;
using namespace glm;
using namespace nta;

Object::Object(vec4 c) : m_color(c) {
}

Object::~Object() {
}

vec2 Object::getCenter() const {
    return vec2(m_body->GetPosition().x, m_body->GetPosition().y);
}

vec2 Object::getTopLeft() const {
    vec2 center = getCenter();
    // assume there's only one fixture and one child
    b2Fixture const * const fixture = m_body->GetFixtureList();
    b2Vec2 extents = fixture->GetAABB(0).GetExtents();

    return vec2(center.x - extents.x, center.y + extents.y);
}

void Object::render_debug(DebugBatch& _) const {
}

void Object::update(const UpdateParams& params) {
    const float planet_width = params.planet->getDimensions().x;
    const float planet_half_width = planet_width/2.f;
    while (getCenter().x > planet_half_width) {
        m_body->SetTransform(b2Vec2(getCenter().x - planet_width, getCenter().y), 
                             m_body->GetAngle());
    }
    while (getCenter().x < -planet_half_width) {
        m_body->SetTransform(b2Vec2(getCenter().x + planet_width, getCenter().y), 
                             m_body->GetAngle());
    }
}