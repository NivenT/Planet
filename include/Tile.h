#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED 

#include <nta/ResourceManager.h>
#include <nta/SpriteBatch.h>

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
    void render(nta::SpriteBatch& batch, glm::vec2 top_left) const {
        glm::vec4 col = color;
        col.a *= active ? 1 : TILE_INACTIVE_ALPHA;
        batch.addGlyph(glm::vec4(top_left, TILE_SIZE, TILE_SIZE), glm::vec4(0,0,1,1), 
                       tex.id, col);
    }
    bool operator!() const {
        return !active;
    }

    nta::GLTexture tex;
    glm::vec4 color;

    bool active = true;
    bool destructable = true;
    bool solid = true;
};

#endif // TILE_H_INCLUDED