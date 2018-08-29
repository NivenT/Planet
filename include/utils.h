#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/DebugBatch.h>
#include <nta/Camera2D.h>

#include "Planet.h"
#include "Object.h"

extern "C" double LambertW(const double z);

extern void debug_render_aabb(nta::DebugBatch& dbatch, const b2AABB& box);
extern void debug_render_poly(nta::DebugBatch& dbatch, const b2PolygonShape* poly,
                              nta::crvec2 position, float rotation);
extern void debug_render_body(nta::DebugBatch& dbatch, const b2Body* body, 
                              bool draw_aabbs);
extern void debug_render_world(nta::DebugBatch& dbatch, const b2World* world, 
                               bool draw_aabbs);
extern float angle(const b2Vec2& a, const b2Vec2& b);
extern glm::vec2 screenToGame(glm::vec2 screen, nta::crvec2 win_dims, const nta::Camera2D& camera, 
                       		  const Planet& planet, bool square_planet);
// Is this the best place for this/
extern const char* get_motion_state_name(ObjectMotionState state);

#endif // UTILS_H_INCLUDED