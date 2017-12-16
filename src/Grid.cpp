#include "Grid.h"

using namespace std;
using namespace glm;

#define PHI(n) (2.f*M_PI/(n))
#define THETA(n) (((n)-2)*M_PI/(n))
#define RADIUS(n) (1.f/sqrt(2.f-2.f*cos(PHI(n))))
#define ROTATE(ang) mat2(cos(ang), sin(ang), -sin(ang), cos(ang))

Grid::Grid() {
    // Really hope I never have to change this code cause it might
    // take me a minute to remember what's going on
    vec2 hex1_center = -TILE_SIZE/2.f * vec2(tan(THETA(12)/2.)+tan(THETA(6)/2.), 0.f);
    vec2 hex2_center = ROTATE(PHI(6)) * hex1_center;
    vec2 sq3_center = -TILE_SIZE/2.f * vec2(0.f, tan(THETA(12)/2.)+tan(THETA(4)/2.));
    vec2 sq2_center = ROTATE(-PHI(6)) * sq3_center;
    vec2 sq1_center = ROTATE(-PHI(6)) * sq2_center;
    
    m_tile.push_back({12, vec2(0.), DODECAGON_COLOR, TILE_SIZE, PHI(12)/2.});
    // M_PI/12. comes from exterior angle (think Gauss-Bonnet)
    m_tile.push_back({4, sq1_center, SQUARE_COLOR, TILE_SIZE, M_PI/12.}); 
    m_tile.push_back({6, hex1_center, HEXAGON_COLOR, TILE_SIZE, PHI(6)/2.});
    m_tile.push_back({4, sq2_center, SQUARE_COLOR, TILE_SIZE, -M_PI/12.});
    m_tile.push_back({6, hex2_center, HEXAGON_COLOR, TILE_SIZE, PHI(6)/2.});
    m_tile.push_back({4, sq3_center, SQUARE_COLOR, TILE_SIZE, PHI(4)/2.});
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

    

