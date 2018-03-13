#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Agent.h"

class Enemy : public Agent {
private:
    nta::GLTexture m_tex;
    glm::vec2 m_extents;
public:
    Enemy(crstring texture, float health = NORMAL_ENEMY_INIT_HEALTH, crvec4 color = glm::vec4(1), 
          uint16_t type = 0);
    ~Enemy();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    //void update(const UpdateParams& params);
};

#endif // ENEMY_H_INCLUDED