#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <memory>

#include <Box2D/Box2D.h>

#include <nta/SpriteBatch.h>
#include <nta/DebugBatch.h>

struct UpdateParams {
};

class Object {
protected:
    //glm::vec2 m_top_left;
    glm::vec4 m_color;
    b2Body* m_body;
public:
    Object(glm::vec4 c);
    virtual ~Object();
    glm::vec2 getCenter() const;
    virtual glm::vec2 getTopLeft() const;
    virtual void add_to_world(b2World* world) = 0;
    virtual void render(nta::SpriteBatch& batch) const = 0;
    virtual void render_debug(nta::DebugBatch& dbatch) const;
    virtual void update(const UpdateParams& params);
};

#endif // OBJECT_H_INCLUDED