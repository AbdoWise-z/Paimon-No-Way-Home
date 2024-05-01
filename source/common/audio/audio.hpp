//
// Created by HIMA on 4/26/2024.
//

#pragma once

#include <irrKlang.h>

using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
namespace our {
    class AudioPlayer {
        static ISoundEngine *SoundEngine;
        static AudioPlayer* instance;
    public:
        AudioPlayer();
        ~AudioPlayer();
        ISound* playSound(const char* soundFile, bool looped, float volume);
        static AudioPlayer* getInstance();
        bool isPlaying(const char* soundFile);
        void AudioPlayer::stopAllSounds();
        void AudioPlayer::stopSound(ISoundSource* src);
    };

}
