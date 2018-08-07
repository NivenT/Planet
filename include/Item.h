#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "Agent.h"

struct ItemParams : public CreationParams {
    ItemParams() {
        extents = SMALL_ITEM_EXTENTS;
    }
    nta::utils::Json json() {
        return CreationParams::json().merge({
            {"texture", tex},
            {"script", use_script},
            {"color", {color.r, color.g, color.b, color.a}},
            {"max_speed", {max_speed.x, max_speed.y}}
        });
    }

    std::string tex;
    std::string use_script;
    glm::vec4 color = glm::vec4(1);
    glm::vec2 max_speed = ITEM_MAX_SPEED;
};

class Item : public Object {
private:
    bool m_equipped;
    union {
        Agent* m_owner;
        Planet* m_planet;
    };
    /// Replace with an animation?
    nta::GLTexture m_tex;
    glm::vec2 m_extents;
    std::string m_use_script;
public:
    Item(crvec4 c, crstring texture, crvec2 extents, crstring use = "", crvec2 speed = ITEM_MAX_SPEED,
         uint16_t type = 0);
    Item(crstring texture, crvec2 extents, crstring use = "", crvec2 speed = ITEM_MAX_SPEED,
         uint16_t type = 0);
    Item(const ItemParams& params, uint16_t type = 0);
    ~Item();
    glm::vec2 getExtents() const;
    void add_to_world(b2World* world, const CreationParams& params);
    void render(nta::SpriteBatch& batch) const;
    void render_icon(nta::SpriteBatch& batch, crvec2 top_left, float transparency = 1.0) const;
    void use(const UpdateParams& params);
    void pickup(Agent* owner, b2World* world);

    friend ChaiManager;
};

#endif // ITEM_H_INCLUDED