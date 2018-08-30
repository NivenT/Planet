#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED 

#include <Box2D/Box2D.h>

#include <nta/IOManager.h>

#include <nta/Entity.h>
#include <nta/Component.h>
#include <nta/SpriteBatch.h>
#include <nta/DebugBatch.h>
#include <nta/Animation2D.h>
#include <nta/Json.h>

#include "Planet.h"

#define OBJECT_IS_IDLE(s) ((s == STANDING) || (s == IDLE))
#define OBJECT_ON_GROUND(s) (OBJECT_IS_IDLE(s) || (s == RUNNING))

enum ObjectMotionState {
    STANDING, RUNNING, JUMPING, FALLING, IDLE
};

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
    nta::EntityID entity;
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
private:
    bool m_invisible = false; // set to true if it shouldn't be rendered for any reason
public:
    GraphicsComponent() : nta::Component(COMPONENT_GRAPHICS_LIST_ID) {}
    void receive(const nta::Message& msg);
    virtual void render(nta::SpriteBatch& batch) const = 0;
    void render_debug(nta::DebugBatch& dbatch) const {}
    bool isInvisible() const { return m_invisible; }
};

class ObjectGraphicsComponent : public GraphicsComponent {
protected:
    std::string m_tex_file;

    glm::vec4 m_color;
    glm::vec2 m_top_left;
    glm::vec2 m_extents;
    float m_angle;
public:
    ObjectGraphicsComponent(nta::crstring tex, nta::crvec4 col) : m_tex_file(tex), m_color(col) {}
    virtual void receive(const nta::Message& message);
};

class TextureComponent : public ObjectGraphicsComponent {
private:
    // technically unneccesary since nta::ResourceManager caches textures
    nta::GLTexture m_tex;
public:
    TextureComponent(nta::crstring texture, nta::crvec4 color = glm::vec4(1));
    void render(nta::SpriteBatch& batch) const;
};

class AnimationComponent : public ObjectGraphicsComponent {
private:
    nta::Animation2D m_anim;
    bool m_flipped;
public:
    AnimationComponent(nta::crstring texture, nta::crivec2 anim_dims = glm::ivec2(1), 
                       nta::crvec4 color = glm::vec4(1));
    void render(nta::SpriteBatch& batch) const;
    void receive(const nta::Message& message);
};

class PlanetGraphicsComponent : public GraphicsComponent {
private:
    glm::vec2 getOffset() const;

    std::vector<std::vector<Tile>>* m_tiles = nullptr;
    int m_sea_level;
public:
    PlanetGraphicsComponent() {}
    void render(nta::SpriteBatch& batch) const;
    void render_debug(nta::DebugBatch& dbatch) const;
    void receive(const nta::Message& message);
};

class PhysicsComponent : public nta::Component {
private:
    void handle_collisions(const UpdateParams& params);
protected:
    virtual void setVelocity(nta::crvec2 vel);
    virtual void applyForce(float x, float y);
    virtual void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, nta::EntityID);
    
    const glm::vec2 m_max_speed;
    ObjectMotionState m_motion_state, m_prev_motion_state = FALLING;
    std::size_t m_standing_frames = 0;
    // true when facing right
    bool m_direction;

    b2Body* m_body;
public:
    PhysicsComponent(nta::crvec2 max_speed) : m_max_speed(max_speed), nta::Component(COMPONENT_PHYSICS_LIST_ID) {}
    virtual ~PhysicsComponent() {}

    void destroy(b2World* world);

    glm::vec2 getCenter() const;
    glm::vec2 getVelocity() const;
    virtual glm::vec2 getExtents() const;
    float getOrientation() const;
    float getMass() const;
    
    virtual void add_to_world(b2World* world, const CreationParams& params);
    virtual void update(const UpdateParams& params);
};

#endif // COMPONENTS_H_INCLUDED