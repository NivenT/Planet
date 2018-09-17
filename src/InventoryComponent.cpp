#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void InventoryComponent::render(SpriteBatch& batch, SpriteFont* font) const {
    if (m_inventory.is_empty()) return;
    if (!m_flag) return;

    static const float ALPHA = 0.4;
    static const float BOUNDARY = 2;
    static const float TEXT_WIDTH = (ITEM_ICON_SIZE + BOUNDARY) * 3.f + BOUNDARY;
    static const vec2 DELTA(ITEM_ICON_SIZE + BOUNDARY, 0);

    vec2 pos(BOUNDARY, 100 - MEDIUM_TEXT_HEIGHT - BOUNDARY/2.f);
    const vector<Entity> items{m_inventory.prev(), m_inventory.curr(), m_inventory.next()};
    for (int i = 0; i < items.size(); i++) {
        m_system->get_component<TextureComponent>(items[i]).unwrap().render_icon(batch, pos, i == 1 ? 1.0 : ALPHA);
        pos += DELTA;
    }
    font->drawText(batch, "Inventory", vec4(BOUNDARY/2.f, 100, TEXT_WIDTH, MEDIUM_TEXT_HEIGHT));
}

void InventoryComponent::receive(const nta::Message& msg) {
    switch(msg.type) {
        case MESSAGE_TOGGLE_SHOW_INVENTORY: m_flag = !m_flag; break;
        case MESSAGE_SHIFT_INVENTORY: if (!m_inventory.is_empty()) {
            if ((long)msg.data == 1) {
                m_inventory.advance();
            } else {
                m_inventory.retreat();
            }
            start_countdown();
        } break;
        case MESSAGE_RESOLVE_COLLISION: {
            CollisionParams p = *(CollisionParams*)msg.data;
            auto pickup = m_system->get_component<PickupComponent>(p.other);
            if (pickup.is_some()) {
                pickup.unwrap().pickup(m_system->get_owner(this), p.params.world);
                m_inventory.push_back(m_system->get_owner(&pickup.unwrap()));
                start_countdown();
            }
        } break;
        case MESSAGE_USE_ITEM: if (!m_inventory.is_empty()) {
            m_system->get_component<EffectComponent>(m_inventory.curr()).map([&](auto& cmpn) {
                cmpn.use(*(UpdateParams*)msg.data);
            });
        } break;
    }
}