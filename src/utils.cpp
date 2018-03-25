#include "utils.h"
#include "Object.h"

using namespace glm;
using namespace nta;

void debug_render_aabb(DebugBatch& dbatch, const b2AABB& box) {
    static const int NUM_PIECES = 150;

    b2Vec2 b2_center = box.GetCenter(), b2_extents = box.GetExtents();
    vec2 center(b2_center.x, b2_center.y), extents(b2_extents.x, b2_extents.y);

    for (int x = -1; x <= 1; x += 2) {
        vec2 start = center + vec2(x*extents.x, extents.y),
             end = center + vec2(x*extents.x, -extents.y);
        dbatch.addLine(start, end, NUM_PIECES, DEBUG_BOX2D_AABB_COLOR);
    }
    for (int y = -1; y <= 1; y += 2) {
        vec2 start = center + vec2(-extents.x, y*extents.y),
             end = center + vec2(extents.x, y*extents.y);
        dbatch.addLine(start, end, NUM_PIECES, DEBUG_BOX2D_AABB_COLOR);
    }
}

void debug_render_poly(DebugBatch& dbatch, const b2PolygonShape* poly, vec2 position, float rotation) {
    static const int NUM_PIECES = 150;

    const int count = poly->GetVertexCount();
    for (int i = 0; i < count; i++) {
        vec2 start(poly->m_vertices[i].x, poly->m_vertices[i].y),
             end(poly->m_vertices[(i+1)%count].x, poly->m_vertices[(i+1)%count].y);
        start = vec2(cos(rotation)*start.x-sin(rotation)*start.y,
                     sin(rotation)*start.x+cos(rotation)*start.y) + position;
        end = vec2(cos(rotation)*end.x-sin(rotation)*end.y,
                   sin(rotation)*end.x+cos(rotation)*end.y) + position;
        dbatch.addLine(start, end, NUM_PIECES, DEBUG_BOX2D_COLOR);
    }
}

void debug_render_body(DebugBatch& dbatch, const b2Body* body, bool draw_aabbs) {
    if (!body) return;
    if (((Object*)body->GetUserData())->getObjectType() & PLANET_TYPE) return;
    vec2 pos(body->GetPosition().x, body->GetPosition().y);
    float rot = body->GetAngle();
    
    // Not sure how I feel about this const placement
    b2Fixture const* curr = body->GetFixtureList();
    while (curr) {
        for (int i = 0; i < curr->GetShape()->GetChildCount(); i++) {
            switch (curr->GetType()) {
                case b2Shape::Type::e_polygon:
                    debug_render_poly(dbatch, (const b2PolygonShape*)curr->GetShape(), pos, rot);
                    break;

                default:
                    /// only draw once
                    if (!draw_aabbs) debug_render_aabb(dbatch, curr->GetAABB(i));
                    break;
            }
            if (draw_aabbs) debug_render_aabb(dbatch, curr->GetAABB(i));
        }
        curr = curr->GetNext();
    }
}

void debug_render_world(DebugBatch& dbatch, const b2World* world, bool draw_aabbs) {
    b2Body const* curr = world->GetBodyList();
    while (curr) {
        debug_render_body(dbatch, curr, draw_aabbs);
        curr = curr->GetNext();
    }
}