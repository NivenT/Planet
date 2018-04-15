#include <nta/Logger.h>

#include "Planet.h"
#include "Player.h"

using namespace glm;
using namespace std;

Planet::~Planet() {
    m_tiles.clear();
    m_dimensions = vec2(0);
    m_sea_level = -1;
}

Planet Planet::new_test() {
    Planet test;

    vector<Tile> types;
    types.push_back(Tile(vec4(0.4, 0.7, 0.1, 0.8))); // grass
    types.push_back(Tile(vec4(0.65, 0.85, 1, 0.3))); // sky
    types.push_back(Tile(vec4(0.5, 0.2, 0.1, 0.5))); // dirt

    test.m_dimensions[0] = 28;
    test.m_dimensions[1] = round(2*M_PI*test.m_dimensions[0]);

    test.m_sea_level = 10;
    const int grass_level = 18;

    for (int r = 0; r < test.m_dimensions[0]; r++) {
        test.m_tiles.emplace_back();
        for (int c = 0; c < test.m_dimensions[1]; c++) {
            int type = r >= grass_level ? 2 :
                       r >= test.m_sea_level ? 0 : 1;
            test.m_tiles.back().push_back(types[type]);
            if (type == 1) {
                test.m_tiles.back().back().destructable = false;
                test.m_tiles.back().back().solid = false;
            }
        }
    }

    test.m_tiles[test.m_sea_level][32].active = false;
    test.m_tiles[test.m_sea_level][33].active = false;
    test.m_tiles[test.m_sea_level][31].active = false;
    test.m_tiles[test.m_sea_level][30].active = false;
    test.m_tiles[test.m_sea_level][34].active = false;
    test.m_tiles[test.m_sea_level+1][30].active = false;
    test.m_tiles[test.m_sea_level+1][34].active = false;
    test.m_tiles[test.m_sea_level+1][32].active = false;
    test.m_tiles[test.m_sea_level+1][31].active = false;
    test.m_tiles[test.m_sea_level+1][33].active = false;
    test.m_tiles[test.m_sea_level+2][32].active = false;
    test.m_tiles[test.m_sea_level+2][31].active = false;
    test.m_tiles[test.m_sea_level+2][33].active = false;
    test.m_tiles[test.m_sea_level+3][32].active = false;

    test.m_tiles[test.m_sea_level+3][35].active = false;
    test.m_tiles[test.m_sea_level+3][36].active = false;
    test.m_tiles[test.m_sea_level+2][36].active = false;

    test.m_gravity = vec2(0, -9.81);
    return test;
}

/// TODO: replace with a #define OFFSET or make some of these functions inline
vec2 Planet::getOffset() const {
    return vec2(-m_dimensions[1]*TILE_SIZE/2., m_sea_level*TILE_SIZE);   
}

vec2 Planet::getDimensions() const {
    return vec2(m_dimensions.y, m_dimensions.x) * TILE_SIZE;
}

vec2 Planet::getTileTopLeft(int r, int c) const {
    const vec2 offset = getOffset();
    return offset - (float)r*TILE_DY + (float)c*TILE_DX;
}

vec2 Planet::getTileCenter(int row, int col) const {
    const vec2 offset = getOffset();
    return offset - (float)row*TILE_DY + (float)col*TILE_DX + vec2(TILE_SIZE, -TILE_SIZE)/2.f;
}

b2Vec2 Planet::getGravity() const {
    return b2Vec2(m_gravity.x, m_gravity.y);
}

ivec2 Planet::getCoord(vec2 pos) const {
    // (0,0) should be top-left of top-left tile
    pos -= getOffset();
    pos /= TILE_SIZE;
    return ivec2((int)abs(pos.y), (int)abs(pos.x));
}

const Tile& Planet::getTile(const ivec2& coord) const {
    if (0 <= coord.x && coord.x < rows) {
        return m_tiles[coord.x][coord.y%cols];
    }
    nta::Logger::writeErrorToLog("Planet.getTile: Tried getting nonexistent Tile");
}

bool Planet::validCoord(const ivec2& coord) const {
    return 0 <= coord.x && coord.x < rows && 0 <= coord.y && coord.y < cols;
}

void Planet::remove_tile(const ivec2& coord) {
    if (validCoord(coord) && m_tiles[coord.x][coord.y].destructable) {
        m_tiles[coord.x][coord.y].active = false;

        b2World* world = m_body->GetWorld();
        world->DestroyBody(m_body);
        add_to_world(world);
    }
}

// TODO: Make fewer assumptions about m_sea_level
// TODO: less duplicated code
vector<vector<b2Vec2>> Planet::createOutline() const {
    static const auto invalidHorz = [&](int r, int c) {
        return r == m_sea_level ? !m_tiles[r][c] : m_tiles[r][c].active == m_tiles[r-1][c].active;
    };
    static const auto invalidVert = [&](int r, int c, bool type) {
        const int left = c == 0 ? cols - 1 : c - 1;
        const int right = c == cols - 1 ? 0 : c + 1;
        return (!type && m_tiles[r][left].active == m_tiles[r][c].active) 
                    || (type && m_tiles[r][right].active == m_tiles[r][c].active);
    };

    vector<vector<b2Vec2>> chains;
    vector<b2Vec2> curr;

    // horizontal chains
    for (int r = m_sea_level; r < rows; r++) {
        const vec2 tl = getTileTopLeft(r, 0);
        curr.emplace_back(tl.x, tl.y);

        for (int c = 0; c <= cols; c++) {
            if (c == cols || invalidHorz(r, c)) {
                if (curr.size() > 1) chains.push_back(curr);
                curr.clear();
            }
            if (c != cols) {
                const vec2 tr = getTileTopLeft(r, c) + TILE_DX;
                curr.emplace_back(tr.x, tr.y);
            }
        }
    }
    // bottom row
    const vec2 bl = getTileTopLeft(rows-1, 0) - TILE_DY;
    curr.emplace_back(bl.x, bl.y);
    for (int c = 0; c <= cols; c++) {
        if (c == cols || !m_tiles[rows-1][c]) {
            if (curr.size() > 1) chains.push_back(curr);
            curr.clear();
        }
        if (c != cols) {
            const vec2 br = getTileTopLeft(rows-1, c) + TILE_DX - TILE_DY;
            curr.emplace_back(br.x, br.y);
        }
    }
    /// TODO: Confirm below code is correct (likely not, but easier to check after shovel implementation)
    // vertical chains
    for (int c = 0; c < cols; c++) {
        const vec2 tl = getTileTopLeft(m_sea_level, c);
        curr.emplace_back(tl.x, tl.y);

        for (int r = m_sea_level; r <= rows; r++) {
            if (r == rows || invalidVert(r, c, false)) {
                if (curr.size() > 1) chains.push_back(curr);
                curr.clear();
            }
            if (r != rows) {
                const vec2 bl = getTileTopLeft(r, c) - TILE_DY;
                curr.emplace_back(bl.x, bl.y);
            }
        }
    }
    // right row
    const vec2 tr = getTileTopLeft(m_sea_level, cols-1) + TILE_DX;
    for (int r = m_sea_level; r <= rows; r++) {
        if (r == rows || invalidVert(r, cols-1, true)) {
            if (curr.size() > 1) chains.push_back(curr);
            curr.clear();
        }
        if (r != rows) {
            const vec2 br = getTileTopLeft(r, cols-1) - TILE_DY + TILE_DX;
            curr.emplace_back(br.x, br.y);
        }
    }

    return chains;
}

float Planet::getRadius() const {
    return (m_dimensions[0] - m_sea_level)*TILE_SIZE;
}

float Planet::getHeight() const {
    return m_dimensions[0] * TILE_SIZE;
}

void Planet::add_to_world(b2World* world) {
    b2BodyDef body_def;
    body_def.type = b2_staticBody;
    body_def.position = b2Vec2(0, 0);
    m_body = world->CreateBody(&body_def);

    for (const auto& chain : createOutline()) {
        b2ChainShape chain_shape;
        chain_shape.CreateChain(chain.data(), chain.size());

        b2FixtureDef fixture_def;
        fixture_def.shape = &chain_shape;
        fixture_def.filter.categoryBits = PLANET_CATEGORY_BITS;
        m_body->CreateFixture(&fixture_def);
    }
    
}

void Planet::render(nta::SpriteBatch& batch) const {
    const vec2 offset = getOffset();
    for (int r = 0; r < m_dimensions[0]; r++) {
        for (int c = 0; c < m_dimensions[1]; c++) {
            m_tiles[r][c].render(batch, offset - (float)r*TILE_DY + (float)c*TILE_DX);
        }
    }
}

void Planet::render_debug(nta::DebugBatch& dbatch) const {
    static const size_t NUM_PIECES = 150;
    const vec2 offset = getOffset();

    for (int r = 0; r <= m_dimensions[0]; r++) {
        vec4 color = r == m_sea_level ? vec4(0,1,1,1) : vec4(1);
        vec2 left(offset - (float)r*TILE_DY), 
             right(offset - (float)r*TILE_DY + (float)m_dimensions[1]*TILE_DX);
        //render_line_in_pieces(pbatch, left, right, NUM_PIECES, color);
        dbatch.addLine(left, right, NUM_PIECES, color);
    }
    for (int c = 0; c <= m_dimensions[1]; c++) {
        vec2 top(offset + (float)c*TILE_DX),
             bottom(offset - (float)m_dimensions[0]*TILE_DY + (float)c*TILE_DX);
        dbatch.addLine(top, bottom, NUM_PIECES);
    }
}