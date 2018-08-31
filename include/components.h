#ifndef COMPONENTS_H_INCLUDED
#define COMPONENTS_H_INCLUDED 

#include <Box2D/Box2D.h>

#include <nta/ECS.h>
#include <nta/SpriteFont.h>
#include <nta/DebugBatch.h>
#include <nta/Animation2D.h>
#include <nta/Json.h>

#include "Planet.h"
#include "Cycle.h"
#include "utils.h"

class SavableComponent : public nta::Component {
public:
    SavableComponent(nta::crstring name) : name(name), nta::Component(COMPONENT_SAVE_LIST_ID) {}
    virtual ~SavableComponent() {}
    virtual nta::utils::Json save() const = 0;
    virtual void load(const nta::utils::Json& json) = 0;

    const std::string name;
};

class GraphicsComponent : public nta::Component {
protected:
    bool m_invisible = false; // set to true if it shouldn't be rendered for any reason
public:
    GraphicsComponent(nta::ComponentListID lists = 0) : nta::Component(COMPONENT_GRAPHICS_LIST_ID | lists) {}
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
    ObjectGraphicsComponent(nta::crstring tex, nta::crvec4 col, nta::ComponentListID lists = 0) : m_tex_file(tex), m_color(col), GraphicsComponent(lists) {}
    virtual void receive(const nta::Message& message);
};

class TextureComponent : public ObjectGraphicsComponent {
private:
    // technically unneccesary since nta::ResourceManager caches textures
    nta::GLTexture m_tex;
public:
    TextureComponent(nta::crstring texture, nta::crvec4 color = glm::vec4(1));
    void render(nta::SpriteBatch& batch) const;
    void render_icon(nta::SpriteBatch& batch, nta::crvec2 top_left, float alpha) const;
};

class AnimationComponent : public ObjectGraphicsComponent {
protected:
    nta::Animation2D m_anim;
    MotionAnimation m_anim_params[NUM_MOTION_STATES];
    ObjectMotionState m_motion_state;
    bool m_flipped;
public:
    AnimationComponent(nta::crstring texture, nta::crivec2 anim_dims = glm::ivec2(1), 
                       MotionAnimation anims[] = {}, nta::crvec4 color = glm::vec4(1));
    void render(nta::SpriteBatch& batch) const;
    virtual void step(float dt);
    void receive(const nta::Message& message);
};

class PlayerAnimationComponent : public AnimationComponent {
private:
    float m_vel_x;
public:
    PlayerAnimationComponent(nta::crstring texture, nta::crivec2 anim_dims, MotionAnimation anims[], nta::crvec4 color = glm::vec4(1)) : AnimationComponent(texture, anim_dims, anims, color) {}
    void step(float dt);
    void receive(const nta::Message& message);
};

// This is big. I should probably decompose it into smaller components
class PhysicsComponent : public nta::Component {
private:
    void handle_collisions(const UpdateParams& params);
protected:
    virtual void setVelocity(nta::crvec2 vel);
    virtual void applyForce(float x, float y);

    virtual void resolve_collision(const UpdateParams&, b2ContactEdge*, b2Contact*, nta::EntityID);
    
    const glm::vec2 m_max_speed;
    glm::vec2 m_extents;

    ObjectMotionState m_motion_state, m_prev_motion_state = FALLING;
    std::size_t m_standing_frames = 0;
    // true when facing right
    bool m_direction;

    b2Body* m_body;
public:
    PhysicsComponent(nta::crvec2 max_speed) : m_max_speed(max_speed), nta::Component(COMPONENT_PHYSICS_LIST_ID) {}
    virtual ~PhysicsComponent() {}

    virtual void add_to_world(b2World* world, const CreationParams& params, nta::EntityID owner);
    void destroy(b2World* world);

    glm::vec2 getCenter() const;
    glm::vec2 getTopLeft() const;
    glm::vec2 getVelocity() const;
    float getOrientation() const;
    float getMass() const;
    
    virtual void update(const UpdateParams& params);
    virtual void receive(const nta::Message& message);
};

class SensorPhysicsComponent : public PhysicsComponent {
public:
    SensorPhysicsComponent(nta::crvec2 max_speed) : PhysicsComponent(max_speed) {}
    void add_to_world(b2World* world, const CreationParams& params, nta::EntityID owner);
};

class ControllerComponent : public nta::Component {
public:
    ControllerComponent() : nta::Component(COMPONENT_CONTROLLER_LIST_ID) {}
    virtual void act(const UpdateParams& params) = 0;
};

class PlayerControllerComponent : public ControllerComponent {
private:
    ObjectMotionState m_motion_state = STANDING;
public:
    PlayerControllerComponent() {}
    void act(const UpdateParams& params);
    void receive(const nta::Message&);
};

class HealthComponent : public nta::Component {
private:
    float m_health;
    float m_max_health;
    bool m_show_bar;
    glm::vec3 m_bar_color;

    glm::vec2 m_extents;
    glm::vec2 m_top_left;
public:
    HealthComponent(float init_health, nta::crvec3 col) : m_health(init_health), m_max_health(init_health), m_show_bar(false), m_bar_color(col), nta::Component(COMPONENT_HEALTH_LIST_ID) {}
    void render(nta::SpriteBatch& batch) const;
    void receive(const nta::Message&);
};

class InventoryComponent : public nta::Component {
private:
    void popup();

    Cycle<nta::EntityID> m_inventory;
    bool m_show_inventory;
    int m_unshow_countdown;
public:
    InventoryComponent() : m_show_inventory(false), m_unshow_countdown(0), nta::Component(COMPONENT_INVENTORY_LIST_ID) {}
    void render(nta::SpriteBatch& batch, nta::SpriteFont* font) const;
    void receive(const nta::Message&);

    void countdown();
};

class PickupComponent : public nta::Component {
private:
    nta::EntityID m_owner = NTA_INVALID_ID;
    bool m_picked_up = false;

    b2Body* m_body = nullptr;
public:
    PickupComponent() : nta::Component(COMPONENT_PICKUP_LIST_ID) {}
    void pickup(nta::EntityID owner, b2World* world);
    void receive(const nta::Message&);
};

#endif // COMPONENTS_H_INCLUDED