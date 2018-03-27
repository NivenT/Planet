#include "Object.h"

using namespace std;
using namespace glm;
using namespace nta;

Object::Object(crvec4 c, uint16_t type) : m_color(c), m_type_mask(type | OBJECT_TYPE) {
}

Object::~Object() {
}

uint16_t Object::getObjectType() const {
    return m_type_mask;
}

vec2 Object::getCenter() const {
    return vec2(m_body->GetPosition().x, m_body->GetPosition().y);
}

float Object::getOrientation() const {
    return m_body->GetAngle();
}

vec2 Object::getTopLeft() const {
    const vec2 center = getCenter();
    // assume there's only one fixture and one child (<-- likely not always a true assumption)
    b2Fixture const * const fixture = m_body->GetFixtureList();
    const b2Vec2 extents = fixture->GetAABB(0).GetExtents();

    return vec2(center.x - extents.x, center.y + extents.y);
}

// Call at end of child's add_to_world
void Object::add_to_world(b2World* world, const CreationParams& params) {
    m_body->SetUserData(this);
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