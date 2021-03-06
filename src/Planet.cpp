#include <nta/Logger.h>

#include "Planet.h"

using namespace glm;
using namespace std;

Planet::~Planet() {
    m_tiles.clear();
    m_dimensions = vec2(0);
    m_sea_level = -1;
}

nta::utils::Json Planet::json() const {
    vector<Tile> types;
    // This is horribly inefficient if there are more than a few tile types
    const auto get_tile_type = [&](const Tile& tile) {
        int idx = 0;
        while (idx < types.size() && types[idx] != tile) ++idx;
        if (idx == types.size()) types.push_back(tile);
        return idx;
    };

    nta::utils::Json ret = {
        {"gravity", {m_gravity.x, m_gravity.y}},
        {"dimensions", {rows, cols}},
        {"sea_level", m_sea_level}
    };

    const int num_tiles = rows*cols;
    int streak = 1;
    int last = get_tile_type(m_tiles[0][0]);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (r*r + c*c == 0) continue;
            if (m_tiles[r][c] == types[last]) {
                streak++;
            } else {
                ret["tile layout"].push_back({streak, last});

                last = get_tile_type(m_tiles[r][c]);
                streak = 1;
            }
        }
    }
    ret["tile layout"].push_back({streak, last});

    for (const auto& tile : types) {
        ret["tile types"].push_back(tile.json());
    }
    return ret;
}

Planet Planet::load(const nta::utils::Json& json) {
    Planet ret;

    ret.m_gravity = vec2(json["gravity"][0], json["gravity"][1]);
    ret.m_dimensions = vec2(json["dimensions"][0], json["dimensions"][1]);
    ret.m_sea_level = json["sea_level"];

    vector<Tile> types;
    for (const auto& tile : json["tile types"]) {
        types.push_back(Tile::load(tile));
    }

    ret.m_tiles.resize(ret.rows);
    for (int r = 0; r < ret.rows; r++) {
        ret.m_tiles[r].resize(ret.cols);
    }

    int curr = 0;
    for (const auto& p : json["tile layout"]) {
        for (int i = 0; i < p[0].as_int(); i++, curr++) {
            int r = curr/ret.cols;
            int c = curr%ret.cols;
            ret.m_tiles[r][c] = types[p[1]];
        }
    }
    return ret;
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

    test.m_gravity = vec2(0, -9.81);
    return test;
}

void Planet::clear() {
    m_tiles.clear();
    m_sea_level = 0;
    rows = cols = 0;
}

void Planet::set_num_rows(int num_rows) {
    rows = num_rows;
    cols = ceil(2.f*M_PI*rows);

    m_tiles.resize(rows);
    for (int r = 0; r < rows; r++) {
        m_tiles[r].resize(cols);
    }
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

    //while (pos.y > 0) pos.y -= rows;
    while (pos.x < 0) pos.x += cols;
    return ivec2((-(int)pos.y)/*%rows*/, ((int)pos.x)%cols);
}

const Tile& Planet::getTile(const ivec2& coord) const {
    if (0 <= coord.x && coord.x < rows) {
        return m_tiles[coord.x][coord.y%cols];
    }
    nta::Logger::writeErrorToLog("Planet.getTile: Tried getting nonexistent Tile at (" +
                                 nta::utils::to_string(coord.x) + ", " + nta::utils::to_string(coord.y) + ")");
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

// TODO: less duplicated code
vector<vector<b2Vec2>> Planet::createOutline() const {
    const auto isTileActive = [&](int r, int c) {
        return m_tiles[r][c].active && m_tiles[r][c].solid;
    };
    const auto invalidHorz = [&](int r, int c) {
        return r == 0 ? !isTileActive(r,c) : isTileActive(r,c) == isTileActive(r-1,c);
    };
    const auto invalidVert = [&](int r, int c, bool type) {
        const int left = c == 0 ? cols - 1 : c - 1;
        const int right = c == cols - 1 ? 0 : c + 1;
        return (!type && isTileActive(r,left) == isTileActive(r,c)) 
                    || (type && isTileActive(r,right) == isTileActive(r,c));
    };

    vector<vector<b2Vec2>> chains;
    vector<b2Vec2> curr;

    // horizontal chains
    for (int r = 0; r < rows; r++) {
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
        const vec2 tl = getTileTopLeft(0, c);
        curr.emplace_back(tl.x, tl.y);

        for (int r = 0; r <= rows; r++) {
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
    const vec2 tr = getTileTopLeft(0, cols-1) + TILE_DX;
    for (int r = 0; r <= rows; r++) {
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