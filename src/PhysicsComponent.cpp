#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

vec2 PhysicsComponent::getCenter() const {
    return vec2(m_body->GetPosition().x, m_body->GetPosition().y);
}

vec2 PhysicsComponent::getExtents() const {
    // assume there's only one fixture and one child (<-- likely not always a true assumption)
    b2Fixture const * const fixture = m_body->GetFixtureList();
    const b2Vec2 extents = fixture->GetAABB(0).GetExtents();
    return vec2(extents.x, extents.y);
}

float PhysicsComponent::getOrientation() const {
    return m_body->GetAngle();
}

vec2 PhysicsComponent::getVelocity() const {
    return vec2(m_body->GetLinearVelocity().x, m_body->GetLinearVelocity().y);
}

float PhysicsComponent::getMass() const {
    return m_body->GetMass();
}

void PhysicsComponent::setVelocity(crvec2 vel) {
    m_body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

void PhysicsComponent::applyForce(float x, float y) {
    m_body->ApplyForceToCenter(b2Vec2(x, y), true);
}

/*
// Call at end of child's add_to_world
void PhysicsComponent::add_to_world(b2World* world, const CreationParams& params) {
    m_body->SetUserData(this);
}
*/

void PhysicsComponent::destroy(b2World* world) {
    if (m_body) world->DestroyBody(m_body);
    m_body = nullptr;
}