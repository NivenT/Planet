#ifndef PLANET_H_INCLUDED
#define PLANET_H_INCLUDED

#include "Tile.h"

/*
 * Coordinate system for m_layout
 * (0, 0)         ...         (0,NUM_COLS-1)
 *   .                              .
 *   .                              .
 * (m_sea_level, 0) ... (m_sea_level, NUM_COLS-1) 
 *   .                              .
 *   .                              .
 * (NUM_ROWS-1, 0)  ...  (NUM_ROWS-1, NUM_COLS-1)
 */

class Planet {
private:
    std::vector<Tile> m_tiles;
    // indices into m_tiles 
    std::vector<std::vector<int>> m_layout;
    // rows x columns (i.e. not width by height)
    glm::ivec2 m_dimensions;
    // row coordinate considered sea level on this planet
    int m_sea_level; 
public:
    Planet() {}
    static Planet new_test();
    ~Planet();
    void render(nta::PrimitiveBatch& pbatch);
};

#endif // PLANET_H_INCLUDED