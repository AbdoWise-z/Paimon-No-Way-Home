//
// Created by xAbdoMo on 4/28/2024.
//

#ifndef GFX_LAB_GROUND_SYSTEM_HPP
#define GFX_LAB_GROUND_SYSTEM_HPP

#include "components/Ground.hpp"
#include "paimon-movement.hpp"

namespace our{
    namespace GroundSystem{
        /**
         * will be called once by PaimonMovement System to initialize the target
         * */
        void setPaimonController(PaimonMovement*);

        /**
         * any function that modifies any ground position, should call this to
         * make paimon move to the correct location ..
         * */
        void onGroundMoved(Ground* ground, glm::vec3 world_delta);
    }
}

#endif //GFX_LAB_GROUND_SYSTEM_HPP
