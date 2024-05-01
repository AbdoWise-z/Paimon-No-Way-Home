//
// Created by xAbdoMo on 5/1/2024.
//

#ifndef PAIMON_NO_WAY_HOME_SPLASH_SCREEN_STATE_HPP
#define PAIMON_NO_WAY_HOME_SPLASH_SCREEN_STATE_HPP

#include <cstdlib>
#include "application.hpp"
#include "texture/texture2d.hpp"
#include "texture/texture-utils.hpp"
#include "audio/audio.hpp"

class SplashScreenState : public our::State {
    our::Texture2D* screen_0{};
    our::Texture2D* screen_1{};

    const float screen_0_fade_in_time = 1.5;
    const float screen_0_hold_time = 0.5;
    const float screen_0_fade_out_time = 0.5;
    const float screen_1_fade_in_time = 1.5;
    const float text_anim_dur = 1.2;


    float timer = 0;
    float text_timer = 0;
    bool clickable = false;
    void onInitialize() override {
        screen_0 = our::texture_utils::loadImage("assets/textures/splash/splash_0.png");
        screen_1 = our::texture_utils::loadImage("assets/textures/splash/splash_1.png");

        timer = 0;
        text_timer = 0;
        clickable = false;

        our::AudioPlayer::getInstance()->playSound("assets/sounds/intro/intro.mp3" , false , 0.2);
    }

    void onImmediateGui() override {
        ImGui::Begin("main_menu" , nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoMove
                     );
        ImGui::SetWindowSize({1280,720});
        ImGui::SetWindowPos({0,0});

        float alpha_0 = 0;
        float alpha_1 = 0;

        if (timer < screen_0_fade_in_time){
            alpha_1 = 0;
            alpha_0 = timer / screen_0_fade_in_time;
        } else if (timer >= screen_0_fade_in_time && timer < screen_0_hold_time + screen_0_fade_in_time){
            alpha_0 = 1;
            alpha_1 = 0;
        } else if (timer >= screen_0_hold_time + screen_0_fade_in_time){
            alpha_1 = (timer - screen_0_hold_time - screen_0_fade_in_time) / screen_1_fade_in_time;
            alpha_0 = 1.0f - (timer - screen_0_hold_time - screen_0_fade_in_time) / screen_0_fade_out_time;
        }

        if (alpha_1 > 1) {
            alpha_1 = 1;
            clickable = true;
        }
        if (alpha_1 < 0) alpha_1 = 0;

        if (alpha_0 > 1) alpha_0 = 1;
        if (alpha_0 < 0) alpha_0 = 0;


        ImGui::SetCursorPos({0,0});
        ImGui::Image(
                (void*) screen_0->getOpenGLName(),
                {1280,720},
                {0,1},
                {1,0},
                {1,1,1,alpha_0}
                );

        ImGui::SetCursorPos({0,0});
        ImGui::Image(
                (void*) screen_1->getOpenGLName(),
                {1280,720},
                {0,1},
                {1,0},
                {1,1,1,alpha_1}
                );

        if (clickable) {
            ImGui::SetWindowFontScale(2);
            ImVec2 text_size = ImGui::CalcTextSize("Press any key to start ..");
            ImGui::SetCursorPos({(1280 - text_size.x) / 2, (720 - text_size.y - 50)});
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, (int) abs(
                    ((text_timer / text_anim_dur - 0.5f) * 2.0f * 255))));
            ImGui::Text("Press any key to start ..");
            ImGui::PopStyleColor();
        }


//        ImGui::Begin("DirectX11 Texture Test");
//        ImGui::Text("pointer = %p", screen_0->getOpenGLName());
//        ImGui::Text("size = %d x %d", 400, 400);
//
//
//        ImGui::ColorEdit4("color_multiplier", &color_multipler.x);
//        ImGui::Image((void*) screen_0->getOpenGLName(), ImVec2((float)400, (float)400), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), color_multipler);
//        ImGui::End();

        ImGui::End();
    }

    void onDraw(double deltaTime) override {
        timer += deltaTime;

        if (clickable){
            text_timer += deltaTime;
            if (text_timer > text_anim_dur)
                text_timer -= text_anim_dur;
        }
    }

    void onEvent(){
        if (clickable){
            getApp()->changeState("main-menu");
        }
    }

    void onKeyEvent(int key, int scancode, int action, int mods) override {
        onEvent();
    }

    void onMouseButtonEvent(int button, int action, int mods) override {
        onEvent();
    }

    void onDestroy() override {
        delete screen_0;
        delete screen_1;
    }
};
#endif //PAIMON_NO_WAY_HOME_SPLASH_SCREEN_STATE_HPP
