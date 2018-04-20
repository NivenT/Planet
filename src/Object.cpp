#include "Object.h"
#include "utils.h"

using namespace std;
using namespace glm;
using namespace nta;

Object::Object(float mx, float my, crvec4 c, uint16_t type) : m_max_speed(mx, my), m_color(c), 
    m_type_mask(type | OBJECT_TYPE), m_direction(true) {
}

Object::Object(crvec2 m, crvec4 c, uint16_t type) : m_max_speed(m), m_color(c), 
    m_type_mask(type | OBJECT_TYPE), m_direction(true) {
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

float Object::getMass() const {
    return m_body->GetMass();
}

void Object::setVelocity(crvec2 vel) {
    m_body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

void Object::applyForce(float x, float y) {
    m_body->ApplyForceToCenter(b2Vec2(x, y), true);
}
/*
void Object::applyForce(crvec2 force) {
    applyForce(force.x, force.y);
}
*/

// Call at end of child's add_to_world
void Object::add_to_world(b2World* world, const CreationParams& params) {
    m_body->SetUserData(this);
}

void Object::render_debug(DebugBatch& _) const {
}

void Object::resolve_collision(const UpdateParams& params, b2ContactEdge* edge, b2Contact* contact, 
                               Object* obj) {
    static const float EPS = 1e-1;
    if (contact->IsTouching() && 
            !(contact->GetFixtureA()->IsSensor() || contact->GetFixtureB()->IsSensor())) {
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);

        float c = angle(manifold.points[0]-manifold.points[1], b2Vec2(1,0));
        if (abs(c) >= 0.707) {
            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < getCenter().y - getExtents().y + EPS) {
                    m_is_standing = true;
                }
            }
        }
    }
}

// Should I just move this code direcly into update?
void Object::handle_collisions(const UpdateParams& params) {
    if (!m_body) return;
    
    for (b2ContactEdge* edge = m_body->GetContactList(); edge != nullptr; edge = edge->next) {
        void* object = edge->other->GetUserData();
        b2Contact* contact = edge->contact;
        
        resolve_collision(params, edge, contact, (Object*)object);
    }
}

void Object::update(const UpdateParams& params) {
    if (!m_body) return;
    
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

    m_is_standing = false;
    handle_collisions(params);

    vec2 vel = getVelocity();
    if (abs(vel.x) > m_max_speed.x) {
        vel.x = m_max_speed.x*sign(vel.x);
    }
    if (abs(vel.y) > m_max_speed.y) {
        vel.y = m_max_speed.y*sign(vel.y);
    }
    setVelocity(vel);

    if (abs(vel.x) >= OBJECT_FACING_THRESHOLD) m_direction = vel.x > 0;
}