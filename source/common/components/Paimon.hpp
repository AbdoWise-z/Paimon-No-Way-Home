//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_PAIMON_HPP
#define GFX_LAB_PAIMON_HPP

#include "Ground.hpp"
#include "systems/common.h"

namespace our {

    class Paimon : public Component{
    public:
        float speed = 8;
        float rotateSpeed = 800;
        Ground* ground = nullptr;
        static std::string getID() { return "Paimon"; }
        void deserialize(const nlohmann::json& data) override;
    };

} // our

#endif //GFX_LAB_PAIMON_HPP
