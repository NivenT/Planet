#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <experimental/filesystem>

#include "Agent.h"

using std::experimental::filesystem::v1::is_regular_file;

struct ItemParams : public CreationParams {
    ItemParams() {
        extents = SMALL_ITEM_EXTENTS;
    }
    ItemParams(const CreationParams& super) : CreationParams(super) {}
    nta::utils::Json json() const {
        return CreationParams::json().merge({
            {"texture", tex},
            {"script", use_script},
            {"color", {color.r, color.g, color.b, color.a}},
            {"max_speed", {max_speed.x, max_speed.y}}
        });
    }
    static ItemParams load(const nta::utils::Json& json) {
        ItemParams ret(CreationParams::load(json));
        ret.tex = (std::string)json["texture"];
        ret.use_script = is_regular_file((std::string)json["script"]) ?
                                (std::string)json["script"] : "";
        ret.color = glm::vec4(json["color"][0], json["color"][1],
                              json["color"][2], json["color"][3]);
        ret.max_speed = glm::vec2(json["max_speed"][0], json["max_speed"][1]);
        return ret;
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