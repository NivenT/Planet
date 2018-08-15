#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

//#include <experimental/filesystem>

#include "Agent.h"

//using std::experimental::filesystem::v1::is_regular_file;

struct EnemyParams : public CreationParams {
    EnemyParams() {
        extents = ENEMY_UNIT_EXTENTS * glm::vec2(3,1);
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
    glm::vec2 max_speed = NORMAL_ENEMY_MAX_SPEED;
    float init_health = NORMAL_ENEMY_INIT_HEALTH;
    MotionAnimation anims[OBJECT_NUM_MOTION_STATES];
    glm::ivec2 anim_dims = glm::ivec2(1,1);
    bool natural_direction = true;
};

class Enemy : public Agent {
protected:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);

    //nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_update_script;
public:
    Enemy(crstring texture, crstring update = "", float health = NORMAL_ENEMY_INIT_HEALTH, 
          crvec2 speed = NORMAL_ENEMY_MAX_SPEED, crvec4 color = glm::vec4(1), uint16_t type = 0);
    Enemy(const EnemyParams& params, uint16_t type = 0);
    ~Enemy();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // ENEMY_H_INCLUDED