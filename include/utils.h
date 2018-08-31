#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/IOManager.h>
#include <nta/Entity.h>
#include <nta/DebugBatch.h>
#include <nta/Camera2D.h>
#include <nta/Json.h>

#include "Planet.h"

#define OBJECT_IS_IDLE(s) ((s == STANDING) || (s == IDLE))
#define OBJECT_ON_GROUND(s) (OBJECT_IS_IDLE(s) || (s == RUNNING))

enum ObjectMotionState {
    STANDING, RUNNING, JUMPING, FALLING, IDLE
};

// This name is trash
struct MotionAnimation {
    int start = 0;
    int length = 1;
    float speed = 1;
};

struct UpdateParams {
    UpdateParams() : planet(nullptr), world(nullptr) {
    }

    Planet* planet;
    b2World* world;
    glm::vec2 player_pos;
    float dt;
};

struct CreationParams {
    CreationParams() : planet(nullptr), position(0), extents(0), 
        density(1), friction(1), restitution(0) {
        max_speed = NORMAL_ENEMY_MAX_SPEED;
    }
    virtual nta::utils::Json json() const {
        return {
            {"position", {position.x, position.y}},
            {"extents", {extents.x, extents.y}},
            {"density", density},
            {"friction", friction},
            {"restitution", restitution}
        };
    }
    void save(nta::crstring path) {
        nta::IOManager::writeFileFromBuffer(path, json().dump(2));
    }
    static CreationParams load(const nta::utils::Json& json) {
        CreationParams ret;
        ret.position = glm::vec2(json["position"][0], json["position"][1]);
        ret.extents = glm::vec2(json["extents"][0], json["extents"][1]);
        ret.density = json["density"];
        ret.friction = json["friction"];
        ret.restitution = json["restitution"];
        return ret;
    }

    // The planet an object belongs to
    Planet* planet;
    glm::vec2 position;
    glm::vec2 extents;
    glm::vec2 max_speed;
    float density;
    float friction;
    float restitution;
};

struct CollisionParams {
    UpdateParams params;
    b2ContactEdge* edge;
    b2Contact* contact;
    nta::EntityID other;
};

class Planet;

extern "C" double LambertW(const double z);

extern void debug_render_aabb(nta::DebugBatch& dbatch, const b2AABB& box);
extern void debug_render_poly(nta::DebugBatch& dbatch, const b2PolygonShape* poly,
                              nta::crvec2 position, float rotation);
extern void debug_render_body(nta::DebugBatch& dbatch, const b2Body* body, 
                              bool draw_aabbs);
extern void debug_render_world(nta::DebugBatch& dbatch, const b2World* world, 
                               bool draw_aabbs);
extern float angle(const b2Vec2& a, const b2Vec2& b);
extern glm::vec2 screenToGame(glm::vec2 screen, nta::crvec2 win_dims, const nta::Camera2D& camera, 
                       		  const Planet& planet, bool square_planet);
// Is this the best place for this/
extern const char* get_motion_state_name(ObjectMotionState state);

#endif // UTILS_H_INCLUDED