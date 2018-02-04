#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "Agent.h"

#define SMALL_ITEM (TILE_SIZE/3.f)

#define SMALL_ITEM_EXTENTS glm::vec2(SMALL_ITEM/2.f, SMALL_ITEM/2.f)

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
    Item(crvec4 c, crstring texture, crvec2 extents);
    Item(crstring texture, crvec2 extents);
    ~Item();
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void update(const UpdateParams& params);
};

#endif // ITEM_H_INCLUDED