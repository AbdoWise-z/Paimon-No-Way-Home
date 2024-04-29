#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include "systems/paimon-idle.hpp"
#include "systems/Level-mapping.hpp"
#include "systems/orbital-camera-controller.hpp"
#include "systems/paimon-movement.hpp"
#include "systems/collision.hpp"
#include <random>
#include "audio/audio.hpp"
#include <irrKlang.h>


#include "systems/CollectablesSystem.h"
#include "systems/state-system.hpp"

using namespace irrklang;



#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
ISoundEngine *SoundEngine = createIrrKlangDevice();

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::CollisionSystem collisionSystem;
    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::PaimonIdleSystem paimonIdleSystem;
    our::LevelMapping levelMapping;
    our::OrbitalCameraControllerSystem orbitalCameraControllerSystem;
    our::PaimonMovement paimonMovement;
    our::AudioPlayer* audioPlayer = our::AudioPlayer::getInstance();
    bool gameoverflag = 0;
    our::StateSystem stateSystem;
    our::CollectablesSystem collectablesSystem;

    our::TexturedMaterial* menuMaterial;
    // A material to be used to highlight hovered buttons (we will use blending to create a negative effect).
    our::TintedMaterial * highlightMaterial;
    // A rectangle mesh on which the menu material will be drawn
    our::Mesh* menuRectangle;
    // A rectangle mesh on which the first digit of mora's count material will be drawn
    our::Mesh* digit_0_rectangle;
    // A rectangle mesh on which the last digit of mora's count material will be drawn
    our::Mesh* digit_1_rectangle;
    // A variable to record the time since the state is entered (it will be used for the fading effect).
    float time;
    // An array of the button that we can interact with
    std::array<Button, 6> buttons;
    // size of framebuffer
    glm::ivec2 size;
    // material to be used to draw the first digit in mora's count
    our::TexturedMaterial* digit_0_material;
    // material to be used to draw the last digit in mora's count (yes we will only have 2 digits :v)
    our::TexturedMaterial* digit_1_material;
    // count of mora
    int mora_count = 0;
    void initHUD() {

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }

        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        size = getApp()->getFrameBufferSize();
        initHUD();
        renderer.initialize(size, config["renderer"]);
        our::Events::Init(getApp() , &world);
        stateSystem.init(&world);
        levelMapping.init(getApp() , &world);
        orbitalCameraControllerSystem.init(getApp());
        paimonMovement.init(getApp());
        collisionSystem.init(getApp());
        auto audioAsset =  our::AssetLoader<std::pair<std::string, float>>::get("audio1");
        if (audioAsset) {
            auto music = audioAsset->first;
            auto volume = audioAsset->second;
            //std::cout<< music<<std::endl;
            std::cout<< volume<<std::endl;

            if (!music.empty()) {
                audioPlayer->playSound(music.c_str(), true, volume); // Play a sound with volume 0.5
            }
        }else{
            std::cout<< "audio is not found" <<std::endl;
        }

    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        our::Events::Update((float) deltaTime);
        stateSystem.update(&world , (float) deltaTime);
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        paimonIdleSystem.update(&world, (float)deltaTime);
        //levelMapping.update();
        orbitalCameraControllerSystem.update(&world , (float) deltaTime);
        paimonMovement.update(&world , &levelMapping, (float) deltaTime);


        collisionSystem.update(&world, &renderer, gameoverflag, audioPlayer);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        drawHUD(deltaTime,size);
        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("level-menu");
        }
        auto mouse = getApp()->getMouse();
        auto audioAsset =  our::AssetLoader<std::pair<std::string, float>>::get("audio2");

        if (audioAsset) {
            auto music = audioAsset->first;
            auto volume = audioAsset->second;
            auto isplaying = audioPlayer->isPlaying(music.c_str());

            if (!music.empty() && mouse.isPressed(0)&& ! isplaying) {
                audioPlayer->playSound(music.c_str(), false, volume);
            }
        }else{
            std::cout<< "audio is not found" <<std::endl;
        }

    }

    void onDestroy() override {
        destroyHUD();
        collectablesSystem.destroy();
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};