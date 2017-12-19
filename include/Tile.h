#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED 

#include "nta/ResourceManager.h"
#include "nta/PrimitiveBatch.h"

#define TILE_SIZE 16.f
#define TILE_DX glm::vec2(TILE_SIZE, 0)
#define TILE_DY glm::vec2(0, TILE_SIZE)

struct Tile {
    nta::GLTexture m_tex;
    glm::vec4 m_color;

    // This is almost certainly Rust influence gettting the better of me
    static Tile new_outline() {
        return {nta::ResourceManager::getTexture("outline.png"), glm::vec4(1,1,1,1)};
    }
    void render(nta::PrimitiveBatch& pbatch, glm::vec2 top_left) const {
        std::vector<nta::Vertex2D> vertices;

        vertices.emplace_back(top_left, m_color, glm::vec2(0,0));
        vertices.emplace_back(top_left + TILE_DX, m_color, glm::vec2(1,0));
        vertices.emplace_back(top_left + TILE_DX - TILE_DY, m_color, glm::vec2(1,1));
        vertices.emplace_back(top_left - TILE_DY, m_color, glm::vec2(0,1));

        pbatch.addPrimitive(std::begin(vertices), std::end(vertices), m_tex.id);
    }
};

#endif // TILE_H_INCLUDED