//
// Created by xAbdoMo on 4/27/2024.
//

#ifndef GFX_LAB_STATEANIMATOR_H
#define GFX_LAB_STATEANIMATOR_H

#include "action-receiver.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace our {

    struct EntityState{
        glm::vec3 position = glm::vec3(0,0,0);
        glm::vec3 scale    = glm::vec3(1,1,1);
        glm::vec3 rotation = glm::vec3(0,0,0);
        glm::vec4 tint     = glm::vec4(1,1,1,1);
        bool enabled = true; //is the entity enabled or not ? (applied at the end)
    };

    class StateAnimator: public ActionReceiver {
    public:
        static std::string getID() { return "State Animator"; }

        // which parameters are we allowed to edit ?
        bool position = true;
        bool scale    = true;
        bool rotation = true;
        bool tint     = true;

        std::vector<EntityState> states;
        int currentState = 0;
        int nextState    = 0;
        float transitionProgress = 0;
        float transitionDuration = 0;

        void deserialize(const nlohmann::json& data) override ;
        void trigger(const nlohmann::json& data) override ;
        std::string getReceiverID() override;
    };

} // our

#endif //GFX_LAB_STATEANIMATOR_H
