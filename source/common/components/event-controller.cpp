//
// Created by xAbdoMo on 4/27/2024.
//

#include "event-controller.h"
#include <iostream>

void our::EventController::deserialize(const nlohmann::json &data) {
    if (!data.is_object()) return;
    auto ev = data["events"];
    if (!ev.is_array()) return;
    for (auto k : ev){
        EventTrigger trigger;
        trigger.type             = static_cast<our::EventType>(k.value("trigger", 0));
        trigger.associatedObject = k.value("object", "");
        trigger.maxTrigger       = k.value("maxTrigger" , trigger.maxTrigger);

        std::vector<EventAction> actions;
        auto act = k["actions"];
        for (auto a : act){
            EventAction aa;
            aa.target          = a.value("target" , "");
            aa.receiverID      = a.value("receiverID" , "");
            aa.triggerCount    = a.value("triggerCount" , 1);
            aa.triggerDelay    = a.value("triggerDelay" , 0.f);
            aa.triggerInterval = a.value("triggerInterval" , 0.f);
            aa.data = a["data"];
            actions.emplace_back(aa);
        }

        events.push_back({trigger , actions});
    }
}


bool our::EventTrigger::operator==(const our::EventTrigger &other) const {
    return (other.associatedObject == this->associatedObject && other.type == this->type);
}
