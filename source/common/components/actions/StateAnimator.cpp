//
// Created by xAbdoMo on 4/27/2024.
//

#include "StateAnimator.h"
#include "deserialize-utils.hpp"

#include <iostream>

namespace our {
    void StateAnimator::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;

        currentState = data.value("initialState" , currentState);
        position     = data.value("position" , position);
        scale        = data.value("scale" , scale);
        rotation     = data.value("rotation" , rotation);
        tint         = data.value("tint" , tint);

        auto states_data = data["states"];
        if (!states_data.is_array()) return;
        for (const auto& state : states_data){
            EntityState s;
            s.position = state.value("position" , s.position);
            s.scale    = state.value("scale"    , s.scale);
            s.rotation = state.value("rotation" , glm::radians(s.rotation));
            s.tint     = state.value("tint"     , s.tint);
            states.emplace_back(s);
        }
    }

    void StateAnimator::trigger(const nlohmann::json &data) {
        if (!data.is_object()) return;

        nextState          = data.value("state" , -1);
        int inc            = data.value("inc" , 0);
        if (inc != 0 && nextState == -1){
            nextState = currentState + inc;
            while (nextState < 0){
                nextState += states.size();
            }
            nextState %= states.size();
        }
        transitionDuration = data.value("duration" , 100.0f);

        if (nextState != -1 && nextState != currentState){
            transitionProgress = 0;
            //std::cout << "Triggered" << std::endl;
        }
    }

    std::string StateAnimator::getReceiverID() {
        return StateAnimator::getID();
    }
} // our