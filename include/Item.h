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
    //std::function<void(Item&, UpdateParams)> m_update_func;
    //std::function<void(const Item&, nta::SpriteBatch&)> m_render_func;
public:
    Item(crvec4 c, crstring texture, crvec2 extents, uint16_t type = 0);
    Item(crstring texture, crvec2 extents, uint16_t type = 0);
    ~Item();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void render_icon(nta::SpriteBatch& batch, crvec2 top_left, float transparency = 1.0) const;
    //void update(const UpdateParams& params);
    void pickup(Agent* owner, b2World* world);
};

#endif // ITEM_H_INCLUDED