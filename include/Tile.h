#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED 

#include <nta/ResourceManager.h>
#include <nta/SpriteBatch.h>
#include <nta/json.h>

#include "defs.h"

struct Tile {
    Tile() {}
    Tile(nta::GLTexture tex, crvec4 color) : tex(tex), color(color) {
    }
    Tile(crvec4 color) : color(color) {
        tex.id = 0;
    }
    // This is almost certainly Rust influence gettting the better of me
    static Tile new_outline() {
        // unsafe but meh
        return {nta::ResourceManager::getTexture("outline.png").get_data(), 
                glm::vec4(1,1,1,1)};
    }
    static Tile load(const nta::utils::Json& json) {
        Tile ret;
        if (json["texture"] == "") {
            ret.tex.id = 0;
        } else {
            ret.tex = nta::ResourceManager::getTexture(json["texture"]).get_data();
        }
        ret.color = glm::vec4(json["color"][0], json["color"][1],
                              json["color"][2], json["color"][3]);
        ret.active = json["active"];
        ret.destructable = json["destructable"];
        ret.solid = json["solid"];
        return ret;
    }
    nta::utils::Json json() const {
        return {
            {"texture", tex.id == 0 ? "" : nta::ResourceManager::getFile(tex).get_data_or("")},
            {"color", {color.r, color.g, color.b, color.a}},
            {"active", active},
            {"destructable", destructable},
            {"solid", solid}
        };
    }
    void render(nta::SpriteBatch& batch, glm::vec2 top_left) const {
        glm::vec4 col = color;
        col.a *= active ? 1 : TILE_INACTIVE_ALPHA;
        batch.addGlyph(glm::vec4(top_left, TILE_SIZE, TILE_SIZE), glm::vec4(0,0,1,1), 
                       tex.id, col);
    }
    operator bool() const {
        return active;
    }
    bool operator==(const Tile& rhs) const {
        return tex == rhs.tex && color == rhs.color && active == rhs.active
                && destructable == rhs.destructable && solid == rhs.solid;
    }
    bool operator!=(const Tile& rhs) const {
        return !(*this == rhs);
    }

    nta::GLTexture tex;
    glm::vec4 color;

    bool active = true;
    bool destructable = true;
    bool solid = true;
};

#endif // TILE_H_INCLUDED