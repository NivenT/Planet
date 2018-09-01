#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void GarbageComponent::dump() {
    while (!m_trash.empty()) {
        m_system->delete_entity(m_trash.top());
        m_trash.pop();
    }
}

void GarbageComponent::receive(Message& msg) {
    switch(msg) {
        case MESSAGE_DESTROY_ENTITY: m_trash.push((EntityID)msg.data); break;
    }
}