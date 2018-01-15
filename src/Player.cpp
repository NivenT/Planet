#include "Player.h"

using namespace std;
using namespace glm;
using namespace nta;

Player::Player() : Agent(PLAYER_COLOR, PLAYER_INIT_HEALTH) {
}

Player::~Player() {
}

void Player::add_to_world(b2World* world) {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(PLAYER_INIT_POS.x, PLAYER_INIT_POS.y);
    m_body = world->CreateBody(&body_def);

    b2PolygonShape body_shape;
    body_shape.SetAsBox(PLAYER_HALF_DIMS.x, PLAYER_HALF_DIMS.y);

    b2FixtureDef fixture_def;
    fixture_def.shape = &body_shape;
    fixture_def.density = PLAYER_DENSITY;
    fixture_def.friction = PLAYER_FRICTION;
    m_body->CreateFixture(&fixture_def); 
}

void Player::render(SpriteBatch& batch) const {
    Light light(getCenter(), PLAYER_COLOR, PLAYER_HALF_DIMS.x*1.618);
    light.render(batch);
}

void Player::update(const UpdateParams& params) {
}