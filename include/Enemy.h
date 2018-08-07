#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Agent.h"

struct EnemyParams : public CreationParams {
    EnemyParams() {
        extents = ENEMY_UNIT_EXTENTS * glm::vec2(3,1);
    }
    nta::utils::Json json() const {
        return CreationParams::json().merge({
            {"texture", tex},
            {"script", update_script},
            {"color", {color.r, color.g, color.b, color.a}},
            {"max_speed", {max_speed.x, max_speed.y}},
            {"init_health", init_health}
        });
    }

    std::string tex;
    std::string update_script;
    glm::vec4 color = glm::vec4(1);
    glm::vec2 max_speed = NORMAL_ENEMY_MAX_SPEED;
    float init_health = NORMAL_ENEMY_INIT_HEALTH;
};

class Enemy : public Agent {
protected:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);

    nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_update_script;
public:
    Enemy(crstring texture, crstring update = "", float health = NORMAL_ENEMY_INIT_HEALTH, 
          crvec2 speed = NORMAL_ENEMY_MAX_SPEED, crvec4 color = glm::vec4(1), uint16_t type = 0);
    Enemy(const EnemyParams& params, uint16_t type = 0);
    ~Enemy();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // ENEMY_H_INCLUDED