#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED 

#include <Box2D/Box2D.h>

#include <nta/Component.h>
#include <nta/SpriteBatch.h>
#include <nta/Animation2D.h>
#include <nta/Json.h>

#include "Planet.h"

struct UpdateParams {
    UpdateParams() : planet(nullptr), world(nullptr) {
    }

    Planet* planet;
    b2World* world;
    glm::vec2 player_pos;
    float dt;
};

struct CreationParams {
    CreationParams() : planet(nullptr), position(0), extents(0), 
        density(1), friction(1), restitution(0) {
    }
    virtual nta::utils::Json json() const {
        return {
            {"position", {position.x, position.y}},
            {"extents", {extents.x, extents.y}},
            {"density", density},
            {"friction", friction},
            {"restitution", restitution}
        };
    }
    void save(nta::crstring path) {
        nta::IOManager::writeFileFromBuffer(path, json().dump(2));
    }
    static CreationParams load(const nta::utils::Json& json) {
        CreationParams ret;
        ret.position = glm::vec2(json["position"][0], json["position"][1]);
        ret.extents = glm::vec2(json["extents"][0], json["extents"][1]);
        ret.density = json["density"];
        ret.friction = json["friction"];
        ret.restitution = json["restitution"];
        return ret;
    }

    // The planet an object belongs to
    Planet* planet;
    glm::vec2 position;
    glm::vec2 extents;
    float density;
    float friction;
    float restitution;
};

class SavableComponent : public nta::Component {
public:
    SavableComponent(nta::crstring name) : name(name), nta::Component(COMPONENT_SAVE_LIST_ID) {}
    virtual ~SavableComponent() {}
    virtual nta::utils::Json save() const = 0;
    virtual void load(const nta::utils::Json& json) = 0;

    const std::string name;
};

class GraphicsComponent : public nta::Component {
public:
    GraphicsComponent() : nta::Component(COMPONENT_GRAPHICS_LIST_ID) {}
    virtual void render(nta::SpriteBatch& batch) const = 0;
};

class PhysicsComponent : public nta::Component {
protected:
    b2Body* m_body;
public:
    PhysicsComponent() : nta::Component(COMPONENT_PHYSICS_LIST_ID) {}
    virtual ~PhysicsComponent() {}

    void destroy(b2World* world);

    glm::vec2 getCenter() const;
    glm::vec2 getTopLeft() const;
    glm::vec2 getVelocity() const;
    virtual glm::vec2 getExtents() const = 0;
    float getOrientation() const;
    float getMass() const;
    
    virtual void add_to_world(b2World* world, const CreationParams& params) = 0;
    virtual void update(const UpdateParams& params) = 0;
};

#endif // COMPONENTS_H_INCLUDED