#include <nta/InputManager.h>
#include <nta/CallbackManager.h>
#include <nta/Path.h>

#include "ChaiManager.h"

using namespace std;
using namespace glm;
using namespace nta;

void PlayerControllerComponent::act(const UpdateParams& params) {
    if (InputManager::justPressed(SDLK_e)) {
        send(Message(MESSAGE_SHIFT_INVENTORY, (void*)1));
    } else if (InputManager::justPressed(SDLK_q)) {
        send(Message(MESSAGE_SHIFT_INVENTORY, (void*)-1));
    } else if (InputManager::justPressed(SDLK_SPACE)) {
        // I should probably get rid of this, but...
        CallbackManager::setTimeout([](){InputManager::releaseKey(SDLK_SPACE);}, HOLD_TIME);
        send(Message(MESSAGE_USE_ITEM, &params));
    }

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
}

void PlayerControllerComponent::receive(const Message& msg) {
    if (msg == MESSAGE_RECEIVE_MOTION_STATE) {
        m_motion_state = *(ObjectMotionState*)msg.data;
    }
}

void ScriptComponent::act(const UpdateParams& params) {
    if (!utils::Path(m_script).is_file()) return;

    ChaiParams cParams;
    cParams.id = m_system->get_owner(m_id);
    cParams.ecs = m_system;
    cParams.params = params;

    ChaiManager::eval_script(m_script, cParams);
}