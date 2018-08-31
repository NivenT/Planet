#include <nta/InputManager.h>

#include "components.h"

using namespace std;
using namespace glm;
using namespace nta;

void PlayerControllerComponent::act(const UpdateParams& params) {
    /*
    if (InputManager::isPressed(SDLK_e) && !m_inventory.is_empty()) {
        // TODO: Clear event if button released
        if (InputManager::justPressed(SDLK_e)) {
            m_inventory.advance();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_e);}, HOLD_TIME);
        }
        popup(PLAYER_STATE_SHOW_INVENTORY, m_inventory_event_id);
    } else if (InputManager::isPressed(SDLK_q) && !m_inventory.is_empty()) {
        if (InputManager::justPressed(SDLK_q)) {
            m_inventory.retreat();
            CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_q);}, HOLD_TIME);
        }
        popup(PLAYER_STATE_SHOW_INVENTORY, m_inventory_event_id);
    }
    */

    vec2 force(0);
    // Should you have to be standing to move left and right?
    if (InputManager::isPressed(SDLK_d)) {
        force += vec2(PLAYER_FORCE, 0);
    } else if (InputManager::isPressed(SDLK_a)) {
        force -= vec2(PLAYER_FORCE, 0);
    }
    if (OBJECT_ON_GROUND(m_motion_state)) {
        if (InputManager::isPressed(SDLK_w)) {
            force += vec2(0, PLAYER_JUMP_FORCE);
        }
    }
    send(Message(MESSAGE_APPLY_FORCE, &force));
    /*
    if (InputManager::justPressed(SDLK_SPACE) && !m_inventory.is_empty()) {
        CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_SPACE);}, HOLD_TIME);
        m_inventory.curr()->use(params);
    }
    */
}

void PlayerControllerComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_MOTION_STATE) {
        m_motion_state = *(ObjectMotionState*)msg.data;
    }
}