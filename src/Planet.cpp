#include "Planet.h"

using namespace glm;
using namespace std;

Planet::~Planet() {
    m_tiles.clear();
    for (auto& row : m_layout) {
        row.clear();
    }
    m_layout.clear();
    m_dimensions = vec2(0);
    m_sea_level = -1;
}

Planet Planet::new_test() {
    Planet test;
    test.m_tiles.push_back(Tile(vec4(0.4, 0.7, 0.1, 1)));
    test.m_tiles.push_back(Tile(vec4(0.35, 0.55, 1, 1)));
    test.m_tiles.push_back(Tile(vec4(0.5, 0.2, 0.1, 1)));
    test.m_dimensions = ivec2(10, 20);
    test.m_sea_level = 3;

    for (int r = 0; r < test.m_dimensions[0]; r++) {
        test.m_layout.emplace_back();
        for (int c = 0; c < test.m_dimensions[1]; c++) {
            test.m_layout.back().push_back(r >= 8 ? 2 :
                                           r >= test.m_sea_level ? 0 : 1);
        }
    }
    return test;
}

vec2 Planet::getOffset() const {
    return vec2(-m_dimensions[1]*TILE_SIZE/2., m_sea_level*TILE_SIZE);   
}

void Planet::render(nta::PrimitiveBatch& pbatch) const {
    const vec2 offset = getOffset();
    for (int r = 0; r < m_dimensions[0]; r++) {
        for (int c = 0; c < m_dimensions[1]; c++) {
            m_tiles[m_layout[r][c]].render(pbatch, offset - (float)r*TILE_DY + (float)c*TILE_DX);
        }
    }
}

void Planet::render_debug(nta::PrimitiveBatch& pbatch) const {
    const vec2 offset = getOffset();

    for (int r = 0; r <= m_dimensions[0]; r++) {
        vec4 color = r == m_sea_level ? vec4(0,1,1,1) : vec4(1);
        nta::Vertex2D left(offset - (float)r*TILE_DY, color), 
                      right(offset-(float)r*TILE_DY+(float)m_dimensions[1]*TILE_DX, color);
        pbatch.addPrimitive({left, right});
    }
    for (int c = 0; c <= m_dimensions[1]; c++) {
        pbatch.addPrimitive({offset + (float)c*TILE_DX, 
                             offset - (float)m_dimensions[0]*TILE_DY + (float)c*TILE_DX});
    }
}