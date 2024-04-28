//
// Created by xAbdoMo on 4/27/2024.
//

#ifndef GFX_LAB_ACTION_RECEIVER_H
#define GFX_LAB_ACTION_RECEIVER_H

#include "ecs/component.hpp"

namespace our {

    class ActionReceiver : public Component {
    public:
        static std::string getID() { return "THIS SHOULD NEVER HAVE AN ID"; }


        void deserialize(const nlohmann::json& data) override = 0;
        virtual void trigger(const nlohmann::json& data) = 0;
        virtual std::string getReceiverID() = 0;
    };

} // our

#endif //GFX_LAB_ACTION_RECEIVER_H
