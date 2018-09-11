#include "World.h"

using namespace std;
using namespace glm;
using namespace nta;

World::World(const Planet& planet) : m_planet(planet), m_player(nullptr), m_flags(0),
									 m_world(m_planet.getGravity()) {
	m_planet.add_to_world(&m_world);
}

World::World(const WorldParams& params) : World(params.planet) {
	init();
	for (const auto& item : params.items) {
		add_object(new Item(item), item);
	}
	for (const auto& enemy : params.enemies) {
		add_object(new Enemy(enemy), enemy);
	}
	for (const auto& spawner : params.spawners) {
		add_object(new Spawner(spawner), spawner);
		((Spawner*)m_objects.back())->subscribe(this);
	}
}

World::~World() {
	destroy();
}

const Planet& World::get_planet() const {
	return m_planet;
}

const Player* World::get_player() const {
	return m_player;
}

const b2World* World::get_b2World() const {
	return &m_world;
}

void World::init() {
	m_player = new Player;
	add_object(m_player, CreationParams{});
}

void World::add_object(Object* obj, const CreationParams& params) {
	obj->add_to_world(&m_world, params);
	// May want to log this at some point
	m_objects.push_back(obj);
}

bool World::remove_object(Object* obj) {
	if (obj->getObjectType() & PLAYER_TYPE) return false;
	for (int i = 1; i < m_objects.size(); i++) {
		if (m_objects[i] == obj) {
			obj->destroyBody(&m_world);
			std::swap(m_objects[i], m_objects.back());
            m_objects.pop_back();
            return true;
		}
	}
	return false;
}

void World::set_flags(uint16_t flags) {
	m_flags |= flags;
}

void World::unset_flags(uint16_t flags) {
	m_flags &= ~flags;
}

void World::clear_flags() {
	m_flags = 0;
}

bool World::are_flags_set(uint16_t flags) const {
	return m_flags & flags;
}

void World::render(SpriteBatch& batch, SpriteBatch& overlay_batch,
			 	   SpriteFont* font) const {
	m_planet.render(batch);
	int offset = are_flags_set(WORLD_DONT_DRAW_PLAYER_FLAG) ? 1 : 0;
	for (auto it = m_objects.begin() + offset; it != m_objects.end(); ++it) {
		(*it)->render(batch);
	}
	// Why is this a flag?
	if (offset == 0 && are_flags_set(WORLD_DRAW_PLAYER_EXTRAS_FLAG)) {
		m_player->render_inventory(overlay_batch, font);
		m_player->render_attack(batch);
	}
}

void World::render_debug(DebugBatch& batch) const {
	m_planet.render_debug(batch);
	for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
		(*it)->render_debug(batch);
	}
}

bool World::update(UpdateParams& params) {
	params.planet = &m_planet;
	params.world = &m_world;
	params.player_pos = m_player->getCenter();

	for (size_t i = 0; i < m_objects.size(); i++) {
		m_objects[i]->update(params);
	}
	m_world.Step(params.dt, 6, 2);

	for (size_t i = 0; i < m_objects.size(); i++) {
        const uint16_t type = m_objects[i]->getObjectType();
        if (type & AGENT_TYPE) {
            Agent* curr = (Agent*)m_objects[i];
            if (curr->getHealth() <= 0) {
                if (type & PLAYER_TYPE) {
                    return true;
                } else {
                    // don't wanna delete player
                    curr->destroyBody(&m_world);
                    delete curr;

                    std::swap(m_objects[i], m_objects.back());
                    m_objects.pop_back();
                    i--;
                }
            }
        }
    }
    return false;
}

void World::destroy() {
	for (b2Body* curr = m_world.GetBodyList(); curr; curr = curr->GetNext()) {
		m_world.DestroyBody(curr);
	}
	for (auto object : m_objects) {
		delete object;
	}
	m_objects.clear();
	m_player = nullptr;
	m_flags = 0;
}

void World::onNotify(const Message& msg) {
	if (msg & MESSAGE_SPAWN_ENEMY) {
		EnemyParams params = *(EnemyParams*)msg.data;
		add_object(new Enemy(params), params);
	}
}

/// New World Stuff
NewWorld::NewWorld(const WorldParams& params) : m_world(params.planet.getGravity()) {
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

        m_ecs.add_component(new HealthComponent(enemy.init_health, ENEMY_HEALTH_COLOR),
                            id);
        m_ecs.add_component(new ScriptComponent(enemy.update_script), id);

        float mass = physics->getMass();
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

        m_ecs.add_component(new HealthComponent(spawner.init_health, ENEMY_HEALTH_COLOR),
                            id);

        m_ecs.add_component(new SpawnerComponent(spawner.spawn_rate*TARGET_FPS,
                                                 EnemyParams::load(utils::Json::from_file(spawner.spawn))),
                            id);

        float mass = physics->getMass();
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &spawner.extents), id);
        m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), id);
    }
    add_player();
}

NewWorld::~NewWorld() {
}

vec2 NewWorld::get_player_center() const {
    PhysicsComponent* comp = (PhysicsComponent*)m_ecs.get_component(m_player, COMPONENT_PHYSICS_LIST_ID);
    return comp->getCenter();
}

void NewWorld::add_player() {
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
    m_ecs.add_component(new HealthComponent(PLAYER_INIT_HEALTH, PLAYER_HEATLH_COLOR), 
                        m_player);
    m_ecs.add_component(new InventoryComponent, m_player);

    float mass = physics->getMass();
    m_ecs.broadcast(Message(MESSAGE_TOGGLE_SHOW_HEALTH_BAR), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_EXT, &params.extents), m_player);
    m_ecs.broadcast(Message(MESSAGE_RECEIVE_MASS, &mass), m_player);
}

void NewWorld::render(SpriteBatch& batch, SpriteBatch& overlay_batch, 
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
}

void NewWorld::render_debug(DebugBatch& batch) const {
    m_planet.render_debug(batch);
    for (auto graphics : *m_ecs.get_component_list(COMPONENT_GRAPHICS_LIST_ID)) {
        ((GraphicsComponent*)graphics)->render_debug(batch);
    }
}

bool NewWorld::update(UpdateParams& params) {
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

    return !m_ecs.does_entity_exist(m_player);
}

void NewWorld::onNotify(const nta::Message& msg) {
    
}