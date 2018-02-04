#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "Agent.h"

class Item : public Object {
private:
    //bool m_equipped;
    Agent* m_owner;
    nta::GLTexture m_tex;
    glm::vec2 m_extents;
    //std::function<void(Item&, UpdateParams)> m_update_func;
    //std::function<void(const Item&, nta::SpriteBatch&)> m_render_func;
public:
    Item(crvec4 c, crstring texture, crvec2 extents);
    ~Item();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);
};

#endif // ITEM_H_INCLUDED