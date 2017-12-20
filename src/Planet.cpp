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
    test.m_tiles.push_back(Tile(vec4(0.65, 0.85, 1, 1)));
    test.m_tiles.push_back(Tile(vec4(0.5, 0.2, 0.1, 1)));

    test.m_sea_level = 7;
    test.m_dimensions[0] = 14;
    test.m_dimensions[1] = round(2*M_PI*test.m_dimensions[0]);

    for (int r = 0; r < test.m_dimensions[0]; r++) {
        test.m_layout.emplace_back();
        for (int c = 0; c < test.m_dimensions[1]; c++) {
            test.m_layout.back().push_back(r >= 12 ? 2 :
                                           r >= test.m_sea_level ? 0 : 1);
        }
    }
    return test;
}

vec2 Planet::getOffset() const {
    return vec2(-m_dimensions[1]*TILE_SIZE/2., m_sea_level*TILE_SIZE);   
}

float Planet::getRadius() const {
    return (m_dimensions[0] - m_sea_level)*TILE_SIZE;
}

float Planet::getHeight() const {
    return m_dimensions[0] * TILE_SIZE;
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
    static const float NUM_PIECES = 50;
    const vec2 offset = getOffset();

    for (int r = 0; r <= m_dimensions[0]; r++) {
        vec4 color = r == m_sea_level ? vec4(0,1,1,1) : vec4(1);

        vec2 left(offset - (float)r*TILE_DY), 
             right(offset - (float)r*TILE_DY + (float)m_dimensions[1]*TILE_DX);
        vec2 diff = right - left;
        for (int i = 0; i < NUM_PIECES; i++) {
            nta::Vertex2D here(left + (float)i*diff/NUM_PIECES, color), 
                          there(left + (float)(i+1)*diff/NUM_PIECES, color);
            pbatch.addPrimitive({here, there});
        }
    }
    for (int c = 0; c <= m_dimensions[1]; c++) {
        vec2 top(offset + (float)c*TILE_DX),
             bottom(offset - (float)m_dimensions[0]*TILE_DY + (float)c*TILE_DX);
        vec2 diff = bottom - top;
        for (int i = 0; i < NUM_PIECES; i++) {
            pbatch.addPrimitive({top + (float)i*diff/NUM_PIECES, top + (float)(i+1)*diff/NUM_PIECES});
        }
    }
}