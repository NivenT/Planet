#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED 

#include <nta/ResourceManager.h>
#include <nta/SpriteBatch.h>

#include "defs.h"

struct Tile {
    nta::GLTexture m_tex;
    glm::vec4 m_color;

    Tile() {}
    Tile(nta::GLTexture tex, crvec4 color) : m_tex(tex), m_color(color) {
    }
    Tile(crvec4 color) : m_color(color) {
        m_tex.id = 0;
    }
    // This is almost certainly Rust influence gettting the better of me
    static Tile new_outline() {
        return {nta::ResourceManager::getTexture("outline.png"), glm::vec4(1,1,1,1)};
    }
    void render(nta::SpriteBatch& batch, glm::vec2 top_left) const {
        batch.addGlyph(glm::vec4(top_left, TILE_SIZE, TILE_SIZE), glm::vec4(0,0,1,1), 
                       m_tex.id, m_color);
    }
};

#endif // TILE_H_INCLUDED