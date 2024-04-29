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
#include "audio/audio.hpp"
#include <irrKlang.h>


#include "systems/CollectablesSystem.h"
#include "systems/state-system.hpp"

using namespace irrklang;



#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
ISoundEngine *SoundEngine = createIrrKlangDevice();

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::PaimonIdleSystem paimonIdleSystem;
    our::LevelMapping levelMapping;
    our::OrbitalCameraControllerSystem orbitalCameraControllerSystem;
    our::PaimonMovement paimonMovement;
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

        collectablesSystem.init();
        // First, we create a material for the menu's background and mora's count digits
        menuMaterial = new our::TexturedMaterial();
        digit_0_material = new our::TexturedMaterial();
        digit_1_material = new our::TexturedMaterial();
        // Here, we load the shader that will be used to draw the hud
        menuMaterial->shader = new our::ShaderProgram();
        menuMaterial->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        menuMaterial->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
        menuMaterial->shader->link();
        digit_0_material->shader = new our::ShaderProgram();
        digit_0_material->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        digit_0_material->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
        digit_0_material->shader->link();
        digit_1_material->shader = new our::ShaderProgram();
        digit_1_material->shader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
        digit_1_material->shader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
        digit_1_material->shader->link();
        // Then we load the menu texture
        menuMaterial->texture = our::texture_utils::loadImage("assets/textures/test-ui.png");
        digit_0_material->texture = our::texture_utils::loadImage("assets/textures/numbers/0.png");
        digit_1_material->texture = our::texture_utils::loadImage("assets/textures/numbers/none.png");
        // Initially, the menu material will be black, then it will fade in
        menuMaterial->tint = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        digit_0_material->tint = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        digit_1_material->tint = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        // Then we make it transparent
        menuMaterial->pipelineState.blending.enabled = true;
        digit_0_material->pipelineState.blending.enabled = true;
        digit_1_material->pipelineState.blending.enabled = true;

        // Second, we create a material to highlight the hovered buttons
        highlightMaterial = new our::TintedMaterial();
        // Since the highlight is not textured, we used the tinted material shaders
        highlightMaterial->shader = new our::ShaderProgram();
        highlightMaterial->shader->attach("assets/shaders/tinted.vert", GL_VERTEX_SHADER);
        highlightMaterial->shader->attach("assets/shaders/tinted.frag", GL_FRAGMENT_SHADER);
        highlightMaterial->shader->link();
        // The tint is white since we will subtract the background color from it to create a negative effect.
        highlightMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        // To create a negative effect, we enable blending, set the equation to be subtract,
        // and set the factors to be one for both the source and the destination.
        highlightMaterial->pipelineState.blending.enabled = true;
        highlightMaterial->pipelineState.blending.equation = GL_FUNC_SUBTRACT;
        highlightMaterial->pipelineState.blending.sourceFactor = GL_ONE;
        highlightMaterial->pipelineState.blending.destinationFactor = GL_ONE;

        // Then we create a rectangle whose top-left corner is at the origin and its size is 1x1.
        // Note that the texture coordinates at the origin is (0.0, 1.0) since we will use the
        // projection matrix to make the origin at the the top-left corner of the screen.
        menuRectangle = new our::Mesh({
            {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        },{
            0, 1, 2, 2, 3, 0,
        });
        digit_0_rectangle = new our::Mesh({
            {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        },{
            0, 1, 2, 2, 3, 0,
        });
        digit_1_rectangle = new our::Mesh({
            {{0.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 0.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{0.0f, 1.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        },{
            0, 1, 2, 2, 3, 0,
        });

        // Reset the time elapsed since the state is entered.
        time = 0;

        // Fill the positions, sizes and actions for the menu buttons
        // Note that we use lambda expressions to set the actions of the buttons.
        // A lambda expression consists of 3 parts:
        // - The capture list [] which is the variables that the lambda should remember because it will use them during execution.
        //      We store [this] in the capture list since we will use it in the action.
        // - The argument list () which is the arguments that the lambda should receive when it is called.
        //      We leave it empty since button actions receive no input.
        // - The body {} which contains the code to be executed.
        //TODO: add buttons
    }
    void drawHUD(double deltaTime,glm::ivec2 size) {

        auto& mouse = getApp()->getMouse();
        glm::vec2 mousePosition = mouse.getMousePosition();

        glViewport(0, 0, size.x, size.y);

        // The view matrix is an identity (there is no camera that moves around).
        // The projection matrix applys an orthographic projection whose size is the framebuffer size in pixels
        // so that the we can define our object locations and sizes in pixels.
        // Note that the top is at 0.0 and the bottom is at the framebuffer height. This allows us to consider the top-left
        // corner of the window to be the origin which makes dealing with the mouse input easier.
        glm::mat4 VP = glm::ortho(0.0f, (float)size.x, (float)size.y, 0.0f, 1.0f, -1.0f);
        // The local to world (model) matrix of the background which is just a scaling matrix to make the menu cover the whole
        // window. Note that we defind the scale in pixels.
        glm::mat4 M = glm::scale(glm::mat4(1.0f), glm::vec3((float)size.x, (float)size.y, 1.0f));
        glm::mat4 digit_0_M = glm::scale(glm::mat4(1.0f), glm::vec3(45.0f, 60.0f, 1.0f));
        glm::mat4 digit_1_M = glm::scale(glm::mat4(1.0f), glm::vec3(45.0f, 60.0f, 1.0f));
        glm::mat4 digit_0_T = glm::translate(digit_0_M, glm::vec3(3.8f, 0.7f, 1.0f));
        glm::mat4 digit_1_T = glm::translate(digit_1_M, glm::vec3(2.8f, 0.7f, 1.0f));

        // First, we apply the fading effect.
        time += (float)deltaTime;
        menuMaterial->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));
        digit_0_material->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));
        digit_1_material->tint = glm::vec4(glm::smoothstep(0.00f, 2.00f, time));
        // Then we render the menu background
        // Notice that I don't clear the screen first, since I assume that the menu rectangle will draw over the whole
        // window anyway.
        int digit_0 = mora_count % 10;
        std::string sdigit_0 = "assets/textures/numbers/" + std::to_string(digit_0);
        std::string digit_0_path = sdigit_0 + ".png";
        digit_0_material->texture = our::texture_utils::loadImage(digit_0_path);
        int digit_1 = (mora_count - mora_count % 10) / 10;
        std::string sdigit_1 = "assets/textures/numbers/" + (digit_1 == 0? "none" : std::to_string(digit_1));
        std::string digit_1_path = sdigit_1 + ".png";
        digit_1_material->texture = our::texture_utils::loadImage(digit_1_path);

        menuMaterial->setup();
        menuMaterial->shader->set("transform", VP*M);
        menuRectangle->draw();
        digit_0_material->setup();
        digit_0_material->shader->set("transform", VP*digit_0_T);
        digit_0_rectangle->draw();
        digit_1_material->setup();
        digit_1_material->shader->set("transform", VP*digit_1_T);
        digit_1_rectangle->draw();

        collectablesSystem.update(&world,mora_count,(float)deltaTime,size,time,getApp());

        for(auto& button: buttons){
            if(button.isInside(mousePosition)){
                highlightMaterial->setup();
                highlightMaterial->shader->set("transform", VP*button.getLocalToWorld());
                menuRectangle->draw();
            }
        }

    }
    void destroyHUD() {
        delete menuRectangle;
        delete digit_0_rectangle;
        delete digit_1_rectangle;

        delete menuMaterial->texture;
        delete menuMaterial->shader;
        delete menuMaterial;

        delete digit_0_material->texture;
        delete digit_0_material->shader;
        delete digit_0_material;

        delete digit_1_material->texture;
        delete digit_1_material->shader;
        delete digit_1_material;

        delete highlightMaterial->shader;
        delete highlightMaterial;
    }
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


        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        drawHUD(deltaTime,size);
        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("level-menu");
        }
        if(keyboard.justPressed(GLFW_KEY_F)){
            // If the escape  key is pressed in this frame, go to the play state
            SoundEngine->play2D("assets/sounds/breakout.mp3", true);
        }
        if(keyboard.justPressed(GLFW_KEY_T)){
            // If the escape  key is pressed in this frame, go to the play state
            SoundEngine->play2D("assets/sounds/bleep.mp3", false);
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