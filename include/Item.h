#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "Agent.h"

class Item : public Object {
private:
    bool m_equipped;
    union {
        Agent* m_owner;
        Planet* m_planet;
    };
    nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_use_script;
public:
    Item(crvec4 c, crstring texture, crvec2 extents, crstring use = "", crvec2 speed = ITEM_MAX_SPEED,
         uint16_t type = 0);
    Item(crstring texture, crvec2 extents, crstring use = "", crvec2 speed = ITEM_MAX_SPEED,
         uint16_t type = 0);
    ~Item();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void render_icon(nta::SpriteBatch& batch, crvec2 top_left, float transparency = 1.0) const;
    void use();
    void pickup(Agent* owner, b2World* world);
};

#endif // ITEM_H_INCLUDED