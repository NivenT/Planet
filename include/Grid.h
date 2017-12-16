#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include "nta/PrimitiveBatch.h"

#define TILE_SIZE 10.f // The length of one side of the dodecagon
// TODO: Choose colors that aren't awful
#define DODECAGON_COLOR vec4(0.3, 0.0, 0.5, 1.0)
#define HEXAGON_COLOR vec4(1.0, 0.0, 0.0, 1.0)
#define SQUARE_COLOR vec4(0.8, 0.7, 0.85, 1.0)

struct RegularPolygon {
    std::size_t numSides;
    glm::vec2 center;
    glm::vec4 color;
    float sideLength;
    float orientation;
};

class Grid {
private:
    void render_tile(nta::PrimitiveBatch& pbatch, crvec2 pos);
    
    std::vector<RegularPolygon> m_tile;
public:
    Grid();
    ~Grid();
    void render(nta::PrimitiveBatch& pbatch);
};

#endif // GRID_H_INCLUDED
