#pragma once

#include<systems/common.h>

namespace our {

    class Mora : public Component{
    public:
        std::string name;
        float rotateSpeed = 90;
        Ground* ground = nullptr;
        static std::string getID() { return "Mora"; }
        void deserialize(const nlohmann::json& data) override;
    };

} // our