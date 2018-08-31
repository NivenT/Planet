#include "components.h"
#include "utils.h"

using namespace std;
using namespace glm;
using namespace nta;

vec2 PhysicsComponent::getCenter() const {
    return vec2(m_body->GetPosition().x, m_body->GetPosition().y);
}

vec2 PhysicsComponent::getTopLeft() const {
    vec2 c = getCenter();
    return vec2(c.x - m_extents.x, c.y + m_extents.y);
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

void PhysicsComponent::destroy(b2World* world) {
    if (m_body) world->DestroyBody(m_body);
    m_body = nullptr;
}

void PhysicsComponent::add_to_world(b2World* world, const CreationParams& params,
                                    EntityID owner) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    body_def.fixedRotation = true;
    m_body = world->CreateBody(&body_def);

    m_extents = params.extents;
    b2PolygonShape body_shape;
    body_shape.SetAsBox(params.extents.x, params.extents.y);

    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    fixture_def.restitution = params.restitution;
    m_body->CreateFixture(&fixture_def);

    m_body->SetUserData((void*)owner);
    send(Message(MESSAGE_RECEIVE_BODY, m_body));
}

void PhysicsComponent::resolve_collision(const UpdateParams& params, b2ContactEdge* edge, b2Contact* contact, 
                               EntityID obj) {
    static const float EPS = 1e-1;
    if (contact->IsTouching() && 
            !(contact->GetFixtureA()->IsSensor() || contact->GetFixtureB()->IsSensor())) {
        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);

        float c = angle(manifold.points[0]-manifold.points[1], b2Vec2(1,0));
        if (abs(c) >= 0.707) {
            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < getCenter().y - m_extents.y + EPS) {
                    m_motion_state = STANDING;
                }
            }
        }
    }

    CollisionParams cParams = {params, edge, contact, obj};
    send(Message(MESSAGE_RESOLVE_COLLISION, &cParams));
}

// Should I just move this code direcly into update?
void PhysicsComponent::handle_collisions(const UpdateParams& params) {
    if (!m_body) return;
    
    for (b2ContactEdge* edge = m_body->GetContactList(); edge != nullptr; edge = edge->next) {
        void* object = edge->other->GetUserData();
        b2Contact* contact = edge->contact;
        
        resolve_collision(params, edge, contact, (EntityID)object);
    }
}

void PhysicsComponent::update(const UpdateParams& params) {
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

    tie(m_motion_state, m_prev_motion_state) = make_tuple(FALLING, m_motion_state);
    handle_collisions(params);

    vec2 vel = getVelocity();
    if (abs(vel.x) > m_max_speed.x) {
        vel.x = m_max_speed.x*sign(vel.x);
    }
    if (abs(vel.y) > m_max_speed.y) {
        vel.y = m_max_speed.y*sign(vel.y);
    }
    setVelocity(vel);

    bool prev_direction = m_direction;
    if (abs(vel.x) >= OBJECT_FACING_THRESHOLD) m_direction = vel.x > 0;

    if (m_motion_state == STANDING && abs(vel.x) >= OBJECT_RUNNING_THRESHOLD) {
        m_motion_state = RUNNING;
    } else if (m_motion_state == FALLING && vel.y >= OBJECT_JUMPING_THRESHOLD) {
        m_motion_state = JUMPING;
    } else if (m_motion_state == STANDING && OBJECT_IS_IDLE(m_prev_motion_state)) {
        if (++m_standing_frames >= OBJECT_IDLE_THRESHOLD) {
            m_motion_state = IDLE;
        }
    }

    if (!OBJECT_IS_IDLE(m_motion_state)) m_standing_frames = 0;

    vec2 tl = getTopLeft();
    float ang = getOrientation();
    send(Message(MESSAGE_RECEIVE_TL, &tl));
    send(Message(MESSAGE_RECEIVE_ANG, &ang));
    send(Message(MESSAGE_RECEIVE_VEL, &vel));
    send(Message(MESSAGE_RECEIVE_MOTION_STATE, &m_motion_state));
    if (m_direction != prev_direction) send(Message(MESSAGE_RECEIVE_DIR, &m_direction));
}

void PhysicsComponent::receive(const Message& msg) {
    if (msg == MESSAGE_APPLY_FORCE) {
        vec2 force = *(vec2*)msg.data;
        applyForce(force.x, force.y);
    }
}

void SensorPhysicsComponent::add_to_world(b2World* world, 
                                          const CreationParams& params,
                                          EntityID owner) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(params.position.x, params.position.y);
    m_body = world->CreateBody(&body_def);

    m_extents = params.extents;
    b2PolygonShape body_shape;
    body_shape.SetAsBox(m_extents.x, m_extents.y);

    // Solid fixture for certain collisions (e.g. with ground)
    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = params.density;
    fixture_def.friction = params.friction;
    fixture_def.restitution = params.restitution;
    fixture_def.filter.maskBits = SPAWNER_MASK_BITS;
    m_body->CreateFixture(&fixture_def);

    // Sensor fixture for detecting collisions w/o resolution (e.g. with player)
    b2FixtureDef sensor_def;
    sensor_def.shape = &body_shape;
    sensor_def.isSensor = true;
    m_body->CreateFixture(&sensor_def);

    m_body->SetUserData((void*)owner);
    send(Message(MESSAGE_RECEIVE_BODY, m_body));
}
