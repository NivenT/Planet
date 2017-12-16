#include "Grid.h"

using namespace std;
using namespace glm;

#define PHI(n) (2.f*M_PI/(n))
#define THETA(n) (((n)-2)*M_PI/(n))
#define RADIUS(n) (1.f/sqrt(2.f-2.f*cos(PHI(n))))
#define ROTATE(ang) mat2(cos(ang), sin(ang), -sin(ang), cos(ang))
#define DIRECTION(ang) vec2(cos(ang), sin(ang))

Grid::Grid(int render_range) : m_render_range(render_range) {
    // Really hope I never have to change this code cause it might
    // take me a minute to remember what's going on
    const vec2 hex1_center = -TILE_SIZE/2.f * vec2(tan(THETA(12)/2.)+tan(THETA(6)/2.), 0.f);
    const vec2 hex2_center = ROTATE(PHI(6)) * hex1_center;
    const vec2 sq3_center = -TILE_SIZE/2.f * vec2(0.f, tan(THETA(12)/2.)+tan(THETA(4)/2.));
    const vec2 sq2_center = ROTATE(-PHI(6)) * sq3_center;
    const vec2 sq1_center = ROTATE(-PHI(6)) * sq2_center;
    
    m_tile.push_back({12, vec2(0.), DODECAGON_COLOR, TILE_SIZE, PHI(12)/2.});
    // M_PI/12. comes from exterior angle (think Gauss-Bonnet)
    m_tile.push_back({4, sq1_center, SQUARE_COLOR, TILE_SIZE, M_PI/12.}); 
    m_tile.push_back({6, hex1_center, HEXAGON_COLOR, TILE_SIZE, PHI(6)/2.});
    m_tile.push_back({4, sq2_center, SQUARE_COLOR, TILE_SIZE, -M_PI/12.});
    m_tile.push_back({6, hex2_center, HEXAGON_COLOR, TILE_SIZE, PHI(6)/2.});
    m_tile.push_back({4, sq3_center, SQUARE_COLOR, TILE_SIZE, PHI(4)/2.});

    const float lattice_size = TILE_SIZE * (tan(THETA(12)/2.) + 1.);
    m_lattice[0] = lattice_size * DIRECTION(4.5*PHI(12) + PHI(12)/2.);
    m_lattice[1] = lattice_size * DIRECTION(6.5*PHI(12) + PHI(12)/2.);
}

Grid::~Grid() {
    m_tile.clear();
    m_lattice[0] = m_lattice[1] = vec2(0.);
    m_render_range = 0;
}

void Grid::render_tile(nta::PrimitiveBatch& pbatch, crvec2 pos) {
    for (auto& poly : m_tile) {
        pbatch.addPrimitive(poly.numSides, poly.center + pos, poly.sideLength,
                            poly.color, poly.orientation);
    }
}

void Grid::render(nta::PrimitiveBatch& pbatch) {
    for (int r = -m_render_range; r <= m_render_range; r++) {
        for (int c = -m_render_range; c <= m_render_range; c++) {
            render_tile(pbatch, (float)r*m_lattice[0] + (float)c*m_lattice[1]);
        }
    }
}

    

