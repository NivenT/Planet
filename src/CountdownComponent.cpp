#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void CountdownComponent::countdown() {
    if (m_countdown == 1) {
        m_flag = false;
    }
    m_countdown = std::max(m_countdown-1, 0);
}

void CountdownComponent::start_countdown(int time) {
    m_countdown = time;
    m_flag = true;
}