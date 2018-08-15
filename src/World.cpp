#include "World.h"

using namespace std;
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

void World::onNotify(const Event& event) {
	if (event & EVENT_SPAWN_ENEMY) {
		EnemyParams params = *(EnemyParams*)event.data;
		add_object(new Enemy(params), params);
	}
}