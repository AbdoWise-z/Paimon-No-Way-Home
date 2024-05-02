//
// Created by xAbdoMo on 4/27/2024.
//

#ifndef GFX_LAB_EVENT_CONTROLLER_H
#define GFX_LAB_EVENT_CONTROLLER_H


#include "ecs/component.hpp"
#include "glm/vec3.hpp"
#include "components/actions/action-receiver.h"

namespace our {
    enum EventType {
        PAIMON_ENTER_GROUND,
        PAIMON_EXIT_GROUND,
        PAIMON_PICK_MORA,
        PAIMON_ENTER_WORLD,
        PAIMON_INTERACT,
        PAIMON_CAMERA_CHANGE
    };

    struct EventTrigger{

        EventType type;
        std::string associatedObject;
        int maxTrigger = -1;

        bool operator==(const EventTrigger& other) const;
    };

    struct EventAction{
        std::string target;       // the target that this event will be delivered to
        std::string receiverID;   // the target component that this event will be delivered to
        float triggerInterval;    // the delay of the consecutive triggers
        int triggerCount;         // the number of times this event is triggered
        float triggerDelay;       // the delay of the first trigger
        nlohmann::json data;      // the payload to target
    };

    struct Event {
        EventTrigger trigger;
        std::vector<EventAction> actions;
    };
}

namespace std {
    template <>
    struct hash<our::EventTrigger> {
        size_t operator()(const our::EventTrigger& obj) const {
            return std::hash<std::string>()(obj.associatedObject);
        }
    };
}

namespace our {
    class EventController : public Component {
    public:
        // The ID of this component type is "DirectionalLight"
        static std::string getID() { return "Event Controller"; }

        // events in the game is just an un-ordered map of list of event actions
        std::vector<Event> events;


        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

} // our


#endif //GFX_LAB_EVENT_CONTROLLER_H
