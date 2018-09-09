#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

//#include <experimental/filesystem>

#include "Agent.h"

//using std::experimental::filesystem::v1::is_regular_file;

class Enemy : public Agent {
protected:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);

    //nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_update_script;
public:
    Enemy(nta::crstring texture, nta::crstring update = "", float health = NORMAL_ENEMY_INIT_HEALTH, 
          nta::crvec2 speed = NORMAL_ENEMY_MAX_SPEED, nta::crvec4 color = glm::vec4(1), uint16_t type = 0);
    Enemy(const EnemyParams& params, uint16_t type = 0);
    ~Enemy();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // ENEMY_H_INCLUDED