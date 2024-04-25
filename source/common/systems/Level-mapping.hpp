//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_LEVEL_MAPPING_HPP
#define GFX_LAB_LEVEL_MAPPING_HPP

#include "ecs/world.hpp"
#include "components/Paimon.hpp"
#include "components/Ground.hpp"
#include "components/camera.hpp"
#include "application.hpp"

#include <glm/gtx/intersect.hpp>
#include <queue>
#include <iostream>

#define EPSILON 1e-2
#define PAIMON_TO_BLOCK_OFFSET 1.0f
#define PAIMON_TO_BLOCK_DIST   1.2f
#define UP_TO_UP_ALIGNMENT     0.999f

#define DIRECTION_ALIGNMENT    0.95f

#define BLOCK_MAX_DISTANCE     2.01f
#define BLOCK_MIN_DISTANCE     1.99000f

#define TYPE2_BLOCK_MAX_DISTANCE    0.010
#define TYPE2_DIRECTION_ALIGNMENT   0.999

#define PUSH(i, k) if (k.first >= 0) {groundMap[i].push_back(k); if (!visitedBlocks[k.first]) next.push(k.first);}

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

    class LevelMapping {
    private:

        inline int findBlockNear(glm::vec3& paimon, glm::vec3& paimonUp, std::vector<bool>& visited) const{
            for (int i = 0;i < blocks.size();i++){
                if (visited[i]) continue;

                auto block = blocks[i];
                if (glm::dot(paimonUp, block.up) < UP_TO_UP_ALIGNMENT) continue;

                auto dis = block.position - paimon + paimonUp * PAIMON_TO_BLOCK_OFFSET;
                if (glm::dot(dis , dis) < PAIMON_TO_BLOCK_DIST){ //a near block
                    return i;
                }
            }
            return -1;
        }

        [[nodiscard]] inline std::pair<int,glm::vec3> findBlockAlongDirection2(
                const glm::vec3& direction,
                const glm::vec3& position,
                const glm::vec3& up,
                const int& ignoreIndex
        ) const{
            int ret = -1;
            float mDepth = 1e10;
            glm::vec3 block_position;
            for (int i = 0;i < blocks.size();i++){
                if (i == ignoreIndex) continue;
                auto block = blocks[i];

                if (glm::dot(block.up , up) < UP_TO_UP_ALIGNMENT) continue;
                auto P0 = block.position;
                auto P1 = position + glm::normalize(direction) * 2.0f;
                auto depth = abs(P0.z); //distance from cam

                auto dot = glm::dot(glm::normalize(P1 - P0) , glm::vec3(0,0,1));
                auto len = glm::length(P1 - P0);
                if (glm::abs(dot) > TYPE2_DIRECTION_ALIGNMENT || len <= TYPE2_BLOCK_MAX_DISTANCE){
                    if (glm::abs(depth) < glm::abs(mDepth)){
                        ret = i;
                        mDepth = depth;
                        block_position = P1;
                    }
                }
            }
            return {ret , block_position};
        }

        [[nodiscard]] inline std::pair<int,glm::vec3> findBlockAlongDirection(
                const glm::vec3& direction,
                const glm::vec3& position,
                const glm::vec3& up
                ) const{

            int res = -1;
            float dFromCam = 1e10;
            glm::vec3 block_position;

            for (int i = 0;i < blocks.size();i++){
                auto block = blocks[i];

                if (glm::dot(block.up , up) < UP_TO_UP_ALIGNMENT) continue;

                auto P0 = block.position;
                auto P1 = position;
                auto pFromCam = abs(P0.z); //distance from cam

                auto dis = P0 - P1;
                auto distance = glm::dot(dis , dis);

                if (distance == 0) continue; // self

                auto directionAlignment = glm::dot(glm::normalize(dis) , glm::normalize(direction));
                distance = glm::sqrt(distance);

                if ((directionAlignment > DIRECTION_ALIGNMENT) &&
                    (distance >= BLOCK_MIN_DISTANCE) &&
                    (distance <= BLOCK_MAX_DISTANCE) &&
                    (pFromCam <= dFromCam)
                        ){ //a near block

                    res = i;
                    dFromCam = pFromCam;
                    block_position = P0;
                    continue;
                }

                if (enableVisualIllusions){
                    /*
                     * L0 = P0 + cam_dir * S0
                     * L1 = P1 + dir     * S1
                     *
                     * X0 = X1 -> cam_dir.x * S0 - dir.x * S1 = P0.x - P1.x
                     * Y0 = Y1 -> cam_dir.y * S0 - dir.y * S1 = P0.y - P1.y
                     *
                     * solve for S0 , S1
                     * sub for z to check
                     *
                     * [cx dx  ppx]     [cx    dx                  ppx                 ]
                     * [cy dy  ppy] --> [0     dy - dx * cy / cx   ppy - ppx * cy / cx ]
                     *
                     * S1 = A0 / A1
                     * S0 = (ppx - dx * A0 / A1) / cx
                     *
                     * but we actually only have cam_dir.z
                     * so the equations are much simpler
                     * */

                    float s1, s0 = 0;
                    if (abs(direction.x) > 0){
                        s1 = (P0.x - P1.x) / direction.x;
                        s0 = (P1.z + direction.z * s1 - P0.z);
                    } else {
                        s1 = (P0.y - P1.y) / direction.y;
                        s0 = (P1.z + direction.z * s1 - P0.z);
                    }

                    P0 = P0 + glm::vec3(0 , 0 , 1) * s0;

                    dis = P0 - P1;

                    distance = glm::dot(dis , dis);
                    if (distance == 0) continue; // self
                    directionAlignment = glm::dot(glm::normalize(dis) , glm::normalize(direction));
                    distance = glm::sqrt(distance);

                    if ((directionAlignment > DIRECTION_ALIGNMENT) &&
                        (distance >= BLOCK_MIN_DISTANCE) &&
                        (distance <= BLOCK_MAX_DISTANCE) &&
                        (pFromCam <= dFromCam)
                            ) {
                        res = i;
                        dFromCam = pFromCam;
                        block_position = P0;
                    }
                }
            }

            return {res , block_position};
        }

        inline bool canStand(glm::vec3& paimon, glm::vec3& ground , glm::vec3& paimonUp, glm::vec3& groundUp) const{
            if (glm::dot(groundUp , paimonUp) < UP_TO_UP_ALIGNMENT) return false;
            auto dis = ground - paimon + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            if (enableVisualIllusions) dis.z = 0;
            return glm::dot(dis , dis) < PAIMON_TO_BLOCK_DIST;
        }



        std::vector<GroundBlock> blocks;
        GroundLinks groundMap;

    public:
        Application* app{};
        Paimon* paimon{};
        CameraComponent* camera{};
        Entity* marker{};

        bool enableVisualIllusions = true; //ignore the y-axis when doing calculations

        void init(Application* a){
            this->app = a;
        }

        float getPaimonDistanceToGround(glm::vec3 block_pos){
            auto PV = camera->getViewMatrix();
            glm::vec3 paimonUp       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
            );

            paimonUp = glm::normalize(paimonUp);

            glm::vec3 paimonPos       = glm::vec3(
                    PV * paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 0 , 0 , 1.0)
            );

            auto dis = block_pos - paimonPos + paimonUp * PAIMON_TO_BLOCK_OFFSET;
            return glm::length(dis);
        }

        std::vector<RoutePart> findRoute(Ground* target){
            if (target == nullptr) return std::vector<RoutePart>();
            auto PV = camera->getViewMatrix();

            glm::vec3 paimonUp       = glm::vec3(
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
            );

            glm::vec3 paimonPosition = glm::vec3(
                    PV *
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0,0,0 , 1.0)
            );

            glm::vec3 paimonViewUp = glm::vec3(PV * glm::vec4(paimonUp , 0.0));
            paimonViewUp = glm::normalize(paimonViewUp);

            std::vector<bool> visited;
            for (auto k : blocks) visited.push_back(false);

            auto initial = findBlockNear(
                    paimonPosition ,
                    paimonViewUp ,
                    visited
            );

            if (blocks[initial].ground == target) return {};

            if (initial == -1) return {};

            visited.clear();
            for (auto k : blocks) visited.push_back(false);

            auto root = new PathTreeNode<std::pair<int,glm::vec3>>();
            root->value = {initial , blocks[initial].position};
            root->parent = nullptr;

            std::queue<PathTreeNode<std::pair<int,glm::vec3>>*> next;
            next.push(root);

            std::vector<PathTreeNode<std::pair<int,glm::vec3>>*> allNodes;
            allNodes.emplace_back(root);

            std::vector<std::pair<int,glm::vec3>> path;
            bool path_found = false;

            while (!next.empty()){
                auto v = next.front();
                next.pop();
                for (auto k : groundMap[v->value.first]){
                    if (visited[k.first]) continue;
                    if (blocks[k.first].ground == target){
                        //found it
                        path.push_back(k);
                        while (v){
                            path.push_back(v->value);
                            v = v->parent;
                        }
                        path_found = true;
                    } else {
                        visited[k.first] = true;
                        auto newNode = new PathTreeNode<std::pair<int,glm::vec3>>();
                        newNode->value = k;
                        newNode->parent = v;
                        allNodes.emplace_back(newNode);
                        next.push(newNode);
                    }

                    if (path_found) break;
                }

                if (path_found) break;
            }

            for (auto k : allNodes){
                delete k;
            }

            if (!path_found) return {};

            std::vector<RoutePart> route;

            for (int i = path.size() - 1;i >= 0;i --){
                RoutePart p = {
                        path[i].first,
                        path[i].second,
                        blocks[i].ground
                };
                route.emplace_back(p);
            }
            return route;
        }

        Ground* ScreenToGroundCast(float screenX, float screenY){
            auto fSx = (float) screenX;
            auto fSy = (float) app->getFrameBufferSize().y - (float) screenY;

            fSx /= (float) app->getFrameBufferSize().x;
            fSx -= 0.5;
            fSx *= 2;

            fSy /= (float) app->getFrameBufferSize().y;
            fSy -= 0.5;
            fSy *= 2;

            //now we have the NDC coords
            glm::vec4 ndcVector = glm::vec4(fSx , fSy , 0 , 1);
            glm::vec4 vsVector  = glm::inverse(camera->getProjectionMatrix(app->getFrameBufferSize())) * ndcVector;

            auto temp = vsVector;
            temp.z = -1;
            glm::vec4 pos = glm::inverse(camera->getViewMatrix()) * vsVector;
            marker->localTransform.position.x = pos.x;
            marker->localTransform.position.y = pos.y;
            marker->localTransform.position.z = pos.z;

            float minDis = 1e15;
            int hitI     = -1;
            int index    = 0;
            for (auto k: blocks){
                float val = 0;

                //currently, I'm doing Sphere ray cast cuz it just easier ..
                if (glm::intersectRaySphere(
                        glm::vec3(vsVector),
                        glm::vec3(0,0,1),
                        k.position,
                        2.0f,
                        val
                        )){

                    if (abs(k.position.z) < minDis){
                        minDis = abs(k.position.z);
                        hitI = index;
                    }
                }

                index++;
            }

            if (hitI != -1) ((DefaultMaterial*) blocks[hitI].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 2 , 2 , 1);
            if (hitI == -1) return nullptr;

            return blocks[hitI].ground;
        }

        std::vector<GroundBlock>& getBlocks(){
            return blocks;
        }

        void update(World *world, float deltaTime) {
            std::vector<Ground*> ground_blocks;
            std::vector<bool> visitedBlocks;

            blocks.clear();
            groundMap.clear();

            //first we need to get all of our objects ready
            for (auto k : world->getEntities()){
                if (k->name == "marker"){
                    marker = k;
                }
                if (paimon == nullptr) paimon = k->getComponent<Paimon>();
                if (camera == nullptr) camera = k->getComponent<CameraComponent>();
                auto g = k->getComponent<Ground>();
                if (g){
                    ground_blocks.emplace_back(g);
                    ((DefaultMaterial*) k->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(1, 0.5 , 0.5 , 1);
                }
            }

            if (!camera || !paimon) return;

            auto PV = camera->getViewMatrix();
            for (auto k : ground_blocks){
                Entity* et = k->getOwner();
                auto localToWorld = et->getLocalToWorldMatrix();
                glm::vec4 pos =  localToWorld * glm::vec4(0, 0, 0 , 1.0);
                pos = PV * pos;
                glm::vec4 up  = PV * et->getLocalToWorldMatrix() * glm::vec4(k->up , 0.0);
                GroundBlock b = {
                        glm::vec3(pos),
                        glm::vec3(up),
                        et,
                        k
                };

                blocks.emplace_back(b);
                visitedBlocks.push_back(false);
            }

            glm::vec3 paimonUp       = glm::vec3(
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0 , 1 , 0 , 0.0)
                    );

            glm::vec3 paimonPosition = glm::vec3(
                    PV *
                    paimon->getOwner()->getLocalToWorldMatrix() *
                    glm::vec4(0,0,0 , 1.0)
                    );

            glm::vec3 paimonViewUp = glm::vec3(PV * glm::vec4(paimonUp , 0.0));
            glm::vec3 left         = glm::vec3(PV * glm::vec4(1,0,0 , 0.0));
            glm::vec3 top          = glm::vec3(PV * glm::vec4(0,1,0 , 0.0));
            glm::vec3 forward      = glm::vec3(PV * glm::vec4(0,0,1 , 0.0));

            paimonViewUp = glm::normalize(paimonViewUp);
            left         = glm::normalize(left);
            forward      = glm::normalize(forward);
            top          = glm::normalize(top);


            std::queue<int> next;
            //add the initial block
            auto initial = findBlockNear(
                    paimonPosition ,
                    paimonViewUp ,
                    visitedBlocks
                    );

            if (initial >= 0)
                next.push(initial);


            glm::vec3 camForward = glm::vec3(
                    glm::mat4(1) * glm::vec4(0,0,1,0)
                    );

            while (!next.empty()){
                auto index = next.front(); next.pop();
                visitedBlocks[index] = true;

                GroundBlock g = blocks[index];

                auto l = findBlockAlongDirection2(left     , g.position , paimonViewUp, index );
                auto r = findBlockAlongDirection2(-left    , g.position , paimonViewUp, index );
                auto f = findBlockAlongDirection2(forward  , g.position , paimonViewUp, index );
                auto b = findBlockAlongDirection2(-forward , g.position , paimonViewUp, index );

                PUSH(index , l);
                PUSH(index , r);
                PUSH(index , f);
                PUSH(index , b);
            }

//            auto it = groundMap.begin();
//            while (it != groundMap.end()){
//                ((DefaultMaterial*) blocks[it->first].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0.5, 1 , 0.5 , 1);
//                it++;
//            }
//
//            //try to find route to the last block
//            auto test = findRoute(ScreenToGroundCast(app->getMouse().getMousePosition().x , app->getMouse().getMousePosition().y));
//            for (auto k : test){
//                ((DefaultMaterial*) blocks[k.blockIndex].et->getComponent<MeshRendererComponent>()->material)->tint = glm::vec4(0, 0 , 1 , 1);
//            }
        }
    };
}

#endif //GFX_LAB_LEVEL_MAPPING_HPP
