#include "World.h"

using namespace std;
using namespace glm;
using namespace nta;

/// New World Stuff
World::World(const WorldParams& params, bool player) : m_world(params.planet.getGravity()) {
    m_planet = params.planet;
    m_planet.add_to_world(&m_world);
    m_ecs.add_component(new GarbageComponent, m_ecs.gen_entity());
    m_ecs.add_component(new EventQueueComponent(&m_world), m_ecs.gen_entity());
    
    for (auto& item : params.items) {
        EntityID id = m_ecs.gen_entity();

        m_ecs.add_component(new PickupComponent, id);

        auto tex = new TextureComponent(item.tex, item.color);
        m_ecs.add_component(tex, id);

        auto physics = new SensorPhysicsComponent(item.max_speed);
        m_ecs.add_component(physics, id);
        physics->add_to_world(&m_world, item, id);

        auto effect = new EffectComponent(item.use_script);
        m_ecs.add_component(effect, id);

        vec2 tl = physics->getTopLeft(), cen = physics->getCenter();
        float ang = physics->getOrientation();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &item.extents), id);
    }
    for (auto& enemy : params.enemies) {
        EntityID id = m_ecs.gen_entity();

        m_ecs.add_component(new AnimationComponent(enemy.tex, enemy.anim_dims,
                                                   enemy.anims, enemy.color),
                            id);

        auto physics = new PhysicsComponent(enemy.max_speed);
        m_ecs.add_component(physics, id);
        physics->add_to_world(&m_world, enemy, id);

        m_ecs.add_component(new HealthComponent(enemy.init_health, ENEMY_HEALTH_CATEGORY,
                                                ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR), 
                            id);
        m_ecs.add_component(new ScriptComponent(enemy.update_script), id);

        vec2 tl = physics->getTopLeft(), cen = physics->getCenter();
        float ang = physics->getOrientation();
        float mass = physics->getMass();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &enemy.extents), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);
    }
    for (auto& spawner : params.spawners) {
        EntityID id = m_ecs.gen_entity();

        m_ecs.add_component(new AnimationComponent(spawner.tex, spawner.anim_dims,
                                                   spawner.anims, spawner.color),
                            id);
        auto physics = new SensorPhysicsComponent(spawner.max_speed);
        m_ecs.add_component(physics, id);
        physics->add_to_world(&m_world, spawner, id);

        m_ecs.add_component(new HealthComponent(spawner.init_health, ENEMY_HEALTH_CATEGORY,
                                                ENEMY_HEALTH_MASK, ENEMY_HEALTH_COLOR),
                            id);

        m_ecs.add_component(new SpawnerComponent(spawner.spawn_rate*TARGET_FPS,
                                                 EnemyParams::load(utils::Json::from_file(spawner.spawn))),
                            id);

        vec2 tl = physics->getTopLeft(), cen = physics->getCenter();
        float ang = physics->getOrientation();
        float mass = physics->getMass();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_TL, &tl), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_CEN, &cen), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_ANG, &ang), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &spawner.extents), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);
    }
    if (player) {
        add_player();
    }
}

World::~World() {
}

vec2 World::get_player_center() const {
    PhysicsComponent* comp = (PhysicsComponent*)m_ecs.get_component(m_player, COMPONENT_PHYSICS_LIST_ID);
    return comp->getCenter();
}

void World::add_player() {
    MotionAnimation temp[] = PLAYER_MOTION_ANIMATIONS;

    m_player = m_ecs.gen_entity();
    m_ecs.add_component(new PlayerAnimationComponent(PLAYER_ANIMATION_FILE, 
                                                     PLAYER_ANIMATION_DIMS,
                                                     temp), 
                        m_player);

    CreationParams params;
    params.position = PLAYER_INIT_POS;
    params.extents = PLAYER_EXTENTS;
    params.max_speed = PLAYER_MAX_SPEED;
    params.density = PLAYER_DENSITY;
    params.friction = PLAYER_FRICTION;
    params.restitution = PLAYER_RESTITUTION;

    auto physics = new PhysicsComponent(params.max_speed);
    m_ecs.add_component(physics, m_player);
    physics->add_to_world(&m_world, params, m_player);

    m_ecs.add_component(new PlayerControllerComponent, m_player);
    m_ecs.add_component(new HealthComponent(PLAYER_INIT_HEALTH, PLAYER_HEALTH_CATEGORY,
                                            PLAYER_HEALTH_MASK, PLAYER_HEATLH_COLOR), 
                        m_player);
    m_ecs.add_component(new InventoryComponent, m_player);
    m_ecs.add_component(new AttackComponent, m_player);

    float mass = physics->getMass();
    m_ecs.broadcast(Message(MESSAGE_TOGGLE_SHOW_HEALTH_BAR), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &params.extents), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), m_player);
}

void World::render(SpriteBatch& batch, SpriteBatch& overlay_batch, 
                      SpriteFont* font) const {
    m_planet.render(batch);
    for (auto graphics : *m_ecs.get_component_list(COMPONENT_GRAPHICS_LIST_ID)) {
        ((GraphicsComponent*)graphics)->render(batch);
    }
    for (auto health : *m_ecs.get_component_list(COMPONENT_HEALTH_LIST_ID)) {
        ((HealthComponent*)health)->render(batch);
    }
    for (auto inventory : *m_ecs.get_component_list(COMPONENT_INVENTORY_LIST_ID)) {
        ((InventoryComponent*)inventory)->render(overlay_batch, font);
    }
    for (auto attack : *m_ecs.get_component_list(COMPONENT_ATTACK_LIST_ID)) {
        ((AttackComponent*)attack)->render(batch);
    }
}

void World::render_debug(DebugBatch& batch) const {
    m_planet.render_debug(batch);
    for (auto graphics : *m_ecs.get_component_list(COMPONENT_GRAPHICS_LIST_ID)) {
        ((GraphicsComponent*)graphics)->render_debug(batch);
    }
}

bool World::update(UpdateParams& params) {
    params.planet = &m_planet;
    params.world = &m_world;
    params.player_pos = get_player_center();

    for (auto controller : *m_ecs.get_component_list(COMPONENT_CONTROLLER_LIST_ID)) {
        ((ControllerComponent*)controller)->act(params);
    }
    for (auto physics : *m_ecs.get_component_list(COMPONENT_PHYSICS_LIST_ID)) {
        ((PhysicsComponent*)physics)->update(params);
    }
    for (auto animation : *m_ecs.get_component_list(COMPONENT_ANIMATION_LIST_ID)) {
        ((AnimationComponent*)animation)->step(params.dt);
    }
    for (auto attack : *m_ecs.get_component_list(COMPONENT_ATTACK_LIST_ID)) {
        ((AttackComponent*)attack)->step(params.dt);
    }
    for (auto count : *m_ecs.get_component_list(COMPONENT_COUNTDOWN_LIST_ID)) {
        ((CountdownComponent*)count)->countdown();
    }
    for (auto queue : *m_ecs.get_component_list(COMPONENT_EVENTQ_LIST_ID)) {
        ((EventQueueComponent*)queue)->process();
    }
    for (auto garbage : *m_ecs.get_component_list(COMPONENT_GARBAGE_LIST_ID)) {
        ((GarbageComponent*)garbage)->dump();
    }
    m_world.Step(params.dt, 6, 2);

    // Should prbably ensure that m_player has a PlayerControllerComponent
    return !m_ecs.does_entity_exist(m_player);
}
