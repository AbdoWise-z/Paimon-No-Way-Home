//
// Created by xAbdoMo on 4/27/2024.
//
#include "events-system-controller.hpp"
#include "components/event-controller.h"
#include "iostream"

static our::Application* mApp;
static our::World* mWorld;

static std::vector<
        std::unordered_map<
            our::EventTrigger,
            std::vector<our::EventAction>
            >
        > events;


struct ActiveAction{
    nlohmann::json data;
    int remainingTriggerCount;
    float nextTriggerDelay;
    float triggerInterval;
    our::ActionReceiver* receiver = nullptr;
};

static std::list<ActiveAction> activeActions;

void our::Events::Init(Application *app, World *world) {
    activeActions.clear();
    events.clear();

    mApp = app;
    mWorld = world;

    for (auto k : world->getEntities()){
        auto comp = k->getComponent<EventController>();
        if (comp != nullptr){
            events.emplace_back(comp->events);
        }
    }
    std::cout << "EVENTS| LOADED: " << events.size() << " event controller" << std::endl;

}

void triggerEven(our::EventType type, const std::string& obj){
    for (const auto& map : events){
        for (const auto& [trigger , actions]: map){
            if (trigger.type == type && obj == trigger.associatedObject){
                // we should trigger this event :)
                for (auto action : actions){
                    ActiveAction activeAction{};
                    activeAction.data = action.data;
                    activeAction.remainingTriggerCount = action.triggerCount;
                    activeAction.nextTriggerDelay = action.triggerDelay;
                    activeAction.triggerInterval = action.triggerInterval;
                    // now search for the receiver
                    for (auto et : mWorld->getEntities()){
                        if (et->name == action.target){
                            auto receivers = et->getAllComponents<our::ActionReceiver>();
                            for (auto receiver : receivers){
                                if (receiver->getReceiverID() == action.receiverID){
                                    activeAction.receiver = receiver;
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    if (activeAction.receiver != nullptr){
                        activeActions.push_back(activeAction);
                    }
                }
            }
        }
    }
}

void our::Events::Update(float deltaTime) {
    for (auto& act : activeActions){
        act.nextTriggerDelay -= deltaTime;
        if (act.nextTriggerDelay < 0){
            //std::cout << "Triggering Event" << std::endl;
            act.receiver->trigger(act.data);
            act.remainingTriggerCount--;
            act.nextTriggerDelay = act.triggerInterval;
        }
    }

    activeActions.remove_if([](const ActiveAction& act) {
       return act.remainingTriggerCount <= 0;
    });
}

void our::Events::onPaimonEnter(our::Ground *g) {
    //std::cout << "Enter Ground" << std::endl;
    triggerEven(EventType::PAIMON_ENTER_GROUND , g->getOwner()->name);
}

void our::Events::onPaimonExit(our::Ground *g) {
    //std::cout << "Exit Ground" << std::endl;
    triggerEven(EventType::PAIMON_EXIT_GROUND , g->getOwner()->name);
}

void our::Events::onPaimonInteract(const std::string& name) {
    triggerEven(EventType::PAIMON_INTERACT , name);
}

void our::Events::onPaimonPickMora(const std::string &mora_name) {
    triggerEven(EventType::PAIMON_PICK_MORA , mora_name);
}

void our::Events::onPaimonCameraChange(const std::string &name) {
    triggerEven(EventType::PAIMON_CAMERA_CHANGE , name);
}

void our::Events::onPaimonEnterWorld() {
    triggerEven(EventType::PAIMON_ENTER_WORLD , "");
    std::cout << "ENTER WORLD" << std::endl;
}






