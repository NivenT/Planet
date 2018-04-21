#ifndef SPAWNER_H_INCLUDED
#define SPAWNER_H_INCLUDED

#include "Enemy.h"

class Spawner : public Enemy {
private:
    // read right -> left: A constant pointer to a constant Enemy
    const Enemy m_spawn;
    std::vector<Object*>& m_objs;
    CreationParams m_params;
    float m_spawn_rate;
    float m_time;
public:
    Spawner(crstring texture, Enemy spawn, std::vector<Object*>& objs,
            float health = NORMAL_SPAWNER_INIT_HEALTH, crvec4 color = glm::vec4(1), 
            uint16_t type = 0);
    void set_creation_params(const CreationParams& params);
    void set_spawn_rate(float rate);
    void add_to_world(b2World* world, const CreationParams& params);
    void update(const UpdateParams& params);
};

#endif // SPAWNER_H_INCLUDED