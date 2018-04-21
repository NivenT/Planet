#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "Agent.h"

class Enemy : public Agent {
protected:
    void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);

    nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_update_script;
public:
    Enemy(crstring texture, crstring update = "", float health = NORMAL_ENEMY_INIT_HEALTH, 
          crvec2 speed = NORMAL_ENEMY_MAX_SPEED, crvec4 color = glm::vec4(1), uint16_t type = 0);
    ~Enemy();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // ENEMY_H_INCLUDED