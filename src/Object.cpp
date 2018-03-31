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

vec2 Object::getExtents() const {
    // assume there's only one fixture and one child (<-- likely not always a true assumption)
    b2Fixture const * const fixture = m_body->GetFixtureList();
    const b2Vec2 extents = fixture->GetAABB(0).GetExtents();
    return vec2(extents.x, extents.y);
}

vec2 Object::getTopLeft() const {
    const vec2 center = getCenter();
    const vec2 extents = getExtents();
    return vec2(center.x - extents.x, center.y + extents.y);
}

vec2 Object::getVelocity() const {
    return vec2(m_body->GetLinearVelocity().x, m_body->GetLinearVelocity().y);
}

void Object::setVelocity(crvec2 vel) {
    m_body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

// Call at end of child's add_to_world
void Object::add_to_world(b2World* world, const CreationParams& params) {
    m_body->SetUserData(this);
}

void Object::render_debug(DebugBatch& _) const {
}

void Object::resolve_collision(const UpdateParams& _, b2ContactEdge* __, b2Contact* ___, Object* ____) {
}

void Object::handle_collisions(const UpdateParams& params) {
    for (b2ContactEdge* edge = m_body->GetContactList(); edge != nullptr; edge = edge->next) {
        void* object = edge->other->GetUserData();
        b2Contact* contact = edge->contact;
        
        resolve_collision(params, edge, contact, (Object*)object);
    }
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

    handle_collisions(params);
}