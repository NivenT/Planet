#include "utils.h"
#include "Object.h"

using namespace glm;
using namespace nta;

void debug_render_aabb(DebugBatch& dbatch, const b2AABB& box) {
    static const int NUM_PIECES = 15;

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

void debug_render_poly(DebugBatch& dbatch, const b2PolygonShape* poly, crvec2 position, float rotation) {
    static const int NUM_PIECES = 15;

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
    //if (body->GetUserData() && (((Object*)body->GetUserData())->getObjectType() & PLANET_TYPE)) return;
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

float angle(const b2Vec2& a, const b2Vec2& b) {
    return (a.x*b.x + a.y*b.y)/sqrt((a.x*a.x + a.y*a.y)*(b.x*b.x+b.y*b.y));
}

// Dependent on code in planet.vert
vec2 screenToGame(vec2 screen, crvec2 win_dims, const Camera2D& camera, 
                  const Planet& planet, bool square_planet) {
    screen *= 2.f/win_dims;
    screen -= 1.f;
    if (!square_planet) {
        const double radius = planet.getRadius()/camera.getDimensions().x;
        const double height = planet.getHeight()/camera.getDimensions().x;

        screen = vec2(screen.y + radius, screen.x);
        double x = height * atan(screen.y/screen.x);
        double z = sqrt(exp(2.0*radius/height)*dot(screen, screen)/(height*height));
        // I never thought the day would come that I make use of the lambertW function
        double y = height * LambertW(z) - radius;
        screen = vec2(x,y);
    }
    return camera.screenToGame(screen);
}

const char* get_motion_state_name(ObjectMotionState state) {
    switch(state) {
        case STANDING: return "STANDING";
        case RUNNING: return "RUNNING";
        case JUMPING: return "JUMPING";
        case FALLING: return "FALLING";
        case IDLE: return "IDLE";
    }
}