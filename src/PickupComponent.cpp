#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void PickupComponent::pickup(EntityID owner, b2World* world) {
    m_owner = owner;
    m_picked_up = true;

    send(Message(MESSAGE_TOGGLE_VISIBILITY));
    m_system->delete_components(m_system->get_owner(this), COMPONENT_PHYSICS_LIST_ID);
    world->DestroyBody(m_body);
    send(Message(MESSAGE_RECEIVE_BODY, nullptr));
}

void PickupComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_BODY) {
        m_body = (b2Body*)msg.data;
    }
}