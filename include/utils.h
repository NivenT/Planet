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
    CreationParams() : position(0), extents(0), 
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

    glm::vec2 position;
    glm::vec2 extents;
    glm::vec2 max_speed;
    float density;
    float friction;
    float restitution;
};

/// \todo Get rid of (probably)
struct EnemyParams : public CreationParams {
    EnemyParams() {
        extents = ENEMY_UNIT_EXTENTS * glm::vec2(3,1);
        max_speed = NORMAL_ENEMY_MAX_SPEED;
    }
    EnemyParams(const CreationParams& super) : CreationParams(super) {}
    nta::utils::Json json() const {
        nta::utils::Json ret = CreationParams::json().merge({
            {"texture", tex},
            {"script", update_script},
            {"color", {color.r, color.g, color.b, color.a}},
            {"max_speed", {max_speed.x, max_speed.y}},
            {"init_health", init_health},
            {"anim_dims", {anim_dims.x, anim_dims.y}},
            {"natural_direction", natural_direction}
        });
        for (int i = 0; i < OBJECT_NUM_MOTION_STATES; i++) {
            ret["anims"].push_back({
                {"start", anims[i].start},
                {"length", anims[i].length},
                {"speed", anims[i].speed}
            });
        }
        return ret;
    }
    static EnemyParams load(const nta::utils::Json& json) {
        EnemyParams ret(CreationParams::load(json));
        ret.tex = (std::string)json["texture"];
        /*
        ret.update_script = is_regular_file((std::string)json["script"]) ?
                                (std::string)json["script"] : "";
        */
        ret.update_script = (std::string)json["script"];
        ret.color = glm::vec4(json["color"][0], json["color"][1],
                              json["color"][2], json["color"][3]);
        ret.max_speed = glm::vec2(json["max_speed"][0], json["max_speed"][1]);
        ret.init_health = json["init_health"];
        ret.anim_dims = glm::ivec2(json["anim_dims"][0], json["anim_dims"][1]);
        for (int i = 0; i < OBJECT_NUM_MOTION_STATES; i++) {
            ret.anims[i].start = json["anims"][i]["start"];
            ret.anims[i].length = json["anims"][i]["length"];
            ret.anims[i].speed = json["anims"][i]["speed"];
        }
        ret.natural_direction = json["natural_direction"];
        return ret;
    }

    std::string tex;
    std::string update_script;
    glm::vec4 color = glm::vec4(1);
    float init_health = NORMAL_ENEMY_INIT_HEALTH;
    MotionAnimation anims[OBJECT_NUM_MOTION_STATES];
    glm::ivec2 anim_dims = glm::ivec2(1,1);
    bool natural_direction = true;
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