#include "Grid.h"

using namespace std;
using namespace glm;

#define PHI(n) (2.f*M_PI/n)
#define THETA(n) ((n-2)*M_PI/n)
#define RADIUS(n) (1.f/sqrt(2.f-2.f*cos(PHI(n))))

Grid::Grid() {
    vec2 hex1_center = -TILE_SIZE/2.f * vec2(tan(THETA(12)/2.)+tan(THETA(6)/2.), 0.f);
    vec2 sq3_center = -TILE_SIZE/2.f * vec2(0.f, tan(THETA(12)/2.)+tan(THETA(4)/2.));
    
    m_tile.push_back({12, vec2(0.), vec4(0.3, 0.0, 0.5, 1.0), TILE_SIZE, PHI(12)/2.});
    m_tile.push_back({6, hex1_center, vec4(1.0, 0.0, 0.0, 1.0), TILE_SIZE, PHI(6)/2.});
    m_tile.push_back({4, sq3_center, vec4(0.8, 0.7, 0.85, 1.0), TILE_SIZE, PHI(4)/2.});
}

Grid::~Grid() {
    m_tile.clear();
}

void Grid::render_tile(nta::PrimitiveBatch& pbatch, crvec2 pos) {
    for (auto& poly : m_tile) {
        pbatch.addPrimitive(poly.numSides, poly.center + pos, poly.sideLength,
                            poly.color, poly.orientation);
    }
}

void Grid::render(nta::PrimitiveBatch& pbatch) {
    render_tile(pbatch, glm::vec2(0.));
}

    

