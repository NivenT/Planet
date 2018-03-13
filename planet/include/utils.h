#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/DebugBatch.h>

extern void debug_render_aabb(nta::DebugBatch& dbatch, const b2AABB& box);
extern void debug_render_poly(nta::DebugBatch& dbatch, const b2PolygonShape* poly,
                              glm::vec2 position, float rotation);
extern void debug_render_body(nta::DebugBatch& dbatch, const b2Body* body, 
                              bool draw_aabbs);
extern void debug_render_world(nta::DebugBatch& dbatch, const b2World* world, 
                               bool draw_aabbs);

#endif // UTILS_H_INCLUDED