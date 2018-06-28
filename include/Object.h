#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <Box2D/Box2D.h>

#include <nta/SpriteBatch.h>
#include <nta/DebugBatch.h>

#include "Planet.h"

class ChaiManager;

struct UpdateParams {
    UpdateParams() : planet(nullptr), world(nullptr) {
    }

    Planet* planet;
    b2World* world;
    float dt;
};

struct CreationParams {
    CreationParams() : planet(nullptr), position(0), extents(0), 
        density(1), friction(1), restitution(0) {
    }

    // The planet an object belongs to
    Planet* planet;
    glm::vec2 position;
    glm::vec2 extents;
    float density;
    float friction;
    float restitution;
};

class RenderAtKey {
    friend class WorldEditor;
    RenderAtKey() {}
    RenderAtKey(const RenderAtKey&);
    RenderAtKey& operator=(const RenderAtKey&);
};

class Object {
private:
    void handle_collisions(const UpdateParams& params);

    const uint16_t m_type_mask;
    const glm::vec2 m_max_speed;
protected:
    virtual void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, Object*);
    virtual void setVelocity(crvec2 vel);
    virtual void applyForce(float x, float y);

    glm::vec4 m_color;
    b2Body* m_body;
    bool m_is_standing;
    // true when facing right
    bool m_direction;
public:
    Object(float mx, float my, crvec4 c, uint16_t type);
    Object(crvec2 m, crvec4 c, uint16_t type);
    virtual ~Object();
    uint16_t getObjectType() const;
    glm::vec2 getCenter() const;
    glm::vec2 getVelocity() const;
    float getOrientation() const;
    float getMass() const;
    bool hasBody() const;
    void destroyBody(b2World* world);
    // (center, half_dims)
    virtual glm::vec2 getTopLeft() const;
    virtual glm::vec2 getExtents() const;
    virtual void add_to_world(b2World* world, const CreationParams& params);
    virtual void render(nta::SpriteBatch& batch) const = 0;
    virtual void render_at(nta::SpriteBatch& batch, crvec2 center, RenderAtKey key);
    virtual void render_debug(nta::DebugBatch& dbatch) const;
    virtual void update(const UpdateParams& params);

    friend ChaiManager;
};

#endif // OBJECT_H_INCLUDED