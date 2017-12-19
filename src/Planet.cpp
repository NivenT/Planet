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
    test.m_tiles.push_back(Tile::new_outline());
    test.m_dimensions = ivec2(10, 20);
    for (int r = 0; r < test.m_dimensions[0]; r++) {
        test.m_layout.emplace_back();
        for (int c = 0; c < test.m_dimensions[1]; c++) {
            test.m_layout.back().push_back(0);
        }
    }
    test.m_sea_level = 7;
    return test;
}

void Planet::render(nta::PrimitiveBatch& pbatch) const {
    // (0,0) should be center of sea level in world coordinates
    vec2 offset(-m_dimensions[1]*TILE_SIZE/2., m_sea_level*TILE_SIZE);
    for (int r = 0; r < m_dimensions[0]; r++) {
        for (int c = 0; c < m_dimensions[1]; c++) {
            m_tiles[m_layout[r][c]].render(pbatch, offset - (float)r*TILE_DY + (float)c*TILE_DX);
        }
    }
}