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

void GarbageComponent::receive(const Message& msg) {
    switch(msg.type) {
        case MESSAGE_DESTROY_ENTITY: m_trash.push(*(Entity*)msg.data); break;
    }
}