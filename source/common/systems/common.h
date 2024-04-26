//
// Created by xAbdoMo on 4/26/2024.
//

#ifndef GFX_LAB_COMMON_H
#define GFX_LAB_COMMON_H

#include <queue>
#include <glm/gtx/intersect.hpp>
#include "../ecs/entity.hpp"
#include "components/Ground.hpp"

namespace our{
    typedef std::pair<int, int> link;

    struct GroundBlock{
        glm::vec3 position;
        glm::vec3 up;
        Entity* et;
        Ground* ground;
    };

    template<typename T>
    struct PathTreeNode{
        PathTreeNode* parent{};
        std::vector<PathTreeNode*> children;
        T value{};
    };

    struct RoutePart {
        int blockIndex;
        glm::vec3 fakePosition;
        Ground* ground;
    };

    typedef std::unordered_map<int , std::vector<std::pair<int,glm::vec3>>> GroundLinks;
}

#endif //GFX_LAB_COMMON_H
