
#pragma once

#include <application.hpp>
#include <shader/shader.hpp>
#include <texture/texture2d.hpp>
#include <texture/texture-utils.hpp>
#include <material/material.hpp>
#include <mesh/mesh.hpp>

#include "audio/audio.hpp"

class LevelMenuState : public our::State{

    our::AudioPlayer* audioPlayer = our::AudioPlayer::getInstance();
    // textures
    std::vector<our::Texture2D*> main_menu_tex;
    our::Texture2D* paimon_icon;
    our::Texture2D* button_style;
    // HUD
    glm::vec2 button_padding = {90,20};
    ImVec2 button_size = {300,300};
    ImVec2 windowPadding = {100,50};
    float fontScale = 15.0f;
    int main_menu_index = 0;
    float accumaltedTime = 0.0f;

    void onInitialize() override {
        for(int i = 0; i < 50; i++) {
            std::string path = "assets/textures/main_menu/frame_" + std::to_string(i);
            path += "_delay-0.1s.png";
            main_menu_tex.push_back(our::texture_utils::loadImage(path));
        }
        paimon_icon = our::texture_utils::loadImage("assets/textures/paimon_icon.png");
        button_style = our::texture_utils::loadImage("assets/textures/button_style.png");
        if(!audioPlayer->isPlaying(our::ost_path)) {
            audioPlayer->playSound(our::ost_path,true,0.5f);
        }
    }

    void onImmediateGui() override {

        static float tempx = 0;
        static float tempy = 0;
        static float posx = 0;
        static float posy = 0;
        auto key = getApp()->getKeyboard();
        if(key.isPressed(GLFW_KEY_1)) {
            tempx +=0.5f;
            std::cout<<"sizex=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_2)) {
            tempy +=0.5f;
            std::cout<<"sizey=" <<tempy<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_3)) {
            tempx -=0.5f;
            std::cout<<"sizex=" <<tempx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_4)) {
            tempy -=0.5f;
            std::cout<<"sizey=" <<tempy<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_5)) {
            posx +=0.5f;
            std::cout<<"posx=" <<posx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_6)) {
            posy +=0.05f;
            std::cout<<"posy=" <<posy<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_7)) {
            posx -=0.5f;
            std::cout<<"posx=" <<posx<<std::endl;
        }
        if(key.isPressed(GLFW_KEY_8)) {
            posy -=0.05f;
            std::cout<<"posy=" <<posy<<std::endl;
        }
        static bool playHoverSound = false;
        static std::vector hover(5,false);

        ImGui::Begin("level-menu" , nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);

        ImGui::SetWindowFontScale(fontScale);
        ImGui::SetWindowSize({1280,720});
        ImGui::SetWindowPos({0,0});

        GLuint id = main_menu_tex[main_menu_index < 50 ? main_menu_index : 99 - main_menu_index]->getOpenGLName();
        ImGui::SetCursorPos({0,0});
        ImGui::Image((void*)id,{1280,720},{0,1},{1,0});

        ImGui::SetCursorPos(windowPadding);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.1f, 0.1f, 0.1f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.15f, 0.15f, 0.5f));
        if(ImGui::Button("1",button_size)) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            audioPlayer->stopSound(our::ost->getSoundSource());
            our::level_path = getApp()->getConfig()["levels"][0].get<std::string>();
            getApp()->changeState("play");
        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            hover[0] = true;
        }else if(!ImGui::IsItemHovered() && !hover[1] && !hover[2] && !hover[3] && !hover[4]){
            playHoverSound = false;
            hover[0] = false;
        }

        float button_style_x = 160.0f;
        float button_style_y = -23.0f;
        float button_style_size_x = 165.0f;
        float button_style_size_y = 160.0f;

        GLuint button_style_id = button_style->getOpenGLName();
        ImGui::SetCursorPos({windowPadding.x + button_style_x, windowPadding.y + button_style_y});
        ImGui::Image((void*)button_style_id,{button_style_size_x,button_style_size_y},{0,1},{1,0});

        float x = windowPadding.x + button_size.x + button_padding.x;
        float y = windowPadding.y;
        ImGui::SetCursorPos({x, y});
        if(ImGui::Button("2",button_size)) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            audioPlayer->stopSound(our::ost->getSoundSource());
            our::level_path = getApp()->getConfig()["levels"][1].get<std::string>();
            getApp()->changeState("play");

        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            hover[1] = true;
        }else if(!ImGui::IsItemHovered() && !hover[0] && !hover[2] && !hover[3] && !hover[4]){
            playHoverSound = false;
            hover[1] = false;
        }

        ImGui::SetCursorPos({x + button_style_x, y + button_style_y});
        ImGui::Image((void*)button_style_id,{button_style_size_x,button_style_size_y},{0,1},{1,0});

        x = windowPadding.x + 2 * button_size.x + 2* button_padding.x;
        y = windowPadding.y;
        ImGui::SetCursorPos({x, y});
        if(ImGui::Button("3",button_size)) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            audioPlayer->stopSound(our::ost->getSoundSource());
            our::level_path = getApp()->getConfig()["levels"][2].get<std::string>();
            getApp()->changeState("play");

        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            hover[2] = true;
        }else if(!ImGui::IsItemHovered() && !hover[1] && !hover[0] && !hover[3] && !hover[4]){
            playHoverSound = false;
            hover[2] = false;
        }

        ImGui::SetCursorPos({x + button_style_x, y + button_style_y});
        ImGui::Image((void*)button_style_id,{button_style_size_x,button_style_size_y},{0,1},{1,0});

        x = windowPadding.x + button_size.x + button_padding.x / 2 - button_size.x / 2;
        y = windowPadding.y + button_size.y + button_padding.y;
        ImGui::SetCursorPos({x,y});
        if(ImGui::Button("4",button_size)) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            audioPlayer->stopSound(our::ost->getSoundSource());
            our::level_path = getApp()->getConfig()["levels"][3].get<std::string>();
            getApp()->changeState("play");

        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            hover[3] = true;
        }else if(!ImGui::IsItemHovered() && !hover[1] && !hover[2] && !hover[0] && !hover[4]){
            playHoverSound = false;
            hover[3] = false;
        }

        ImGui::SetCursorPos({x + button_style_x, y + button_style_y});
        ImGui::Image((void*)button_style_id,{button_style_size_x,button_style_size_y},{0,1},{1,0});

        x = 1280 - x - button_size.x;
        ImGui::SetCursorPos({x,y});
        if(ImGui::Button("5",button_size)) {
            audioPlayer->playSound(our::press_button_audio.first.c_str(),false, our::press_button_audio.second);
            audioPlayer->stopSound(our::ost->getSoundSource());
            our::level_path = getApp()->getConfig()["levels"][4].get<std::string>();
            getApp()->changeState("play");

        }
        if(ImGui::IsItemHovered() && !playHoverSound) {
            audioPlayer->playSound(our::hover_button_audio.first.c_str(),false, our::hover_button_audio.second);
            playHoverSound = true;
            hover[4] = true;
        }else if(!ImGui::IsItemHovered() && !hover[1] && !hover[2] && !hover[3] && !hover[0]){
            playHoverSound = false;
            hover[4] = false;
        }

        ImGui::SetCursorPos({x + button_style_x, y + button_style_y});
        ImGui::Image((void*)button_style_id,{button_style_size_x,button_style_size_y},{0,1},{1,0});

        GLuint paimon_icon_id = paimon_icon->getOpenGLName();
        ImGui::SetCursorPos({1280 - 105 - 30,720 - 140 - 30});
        ImGui::Image((void*)paimon_icon_id,{105,140},{0,1},{1,0});

        ImGui::SetWindowFontScale(3);

        ImGui::SetCursorPos({1280 - 50 - 20,20});
        if(ImGui::Button("X",{50,50})) {
            getApp()->changeState("main-menu");
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::End();

    }

    void onDraw(double deltaTime) override {
        accumaltedTime += deltaTime;
        if(accumaltedTime > 0.1f) {
            accumaltedTime = 0.0f;
            main_menu_index++;
            main_menu_index %= 100;
        }
    }

    void onDestroy() override {
        delete paimon_icon;
        delete button_style;
    }
};


