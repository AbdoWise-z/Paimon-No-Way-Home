//
// Created by HIMA on 4/26/2024.
//
#include <string>
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
        void playSound(const char* soundFile, bool looped, float volume);
        static AudioPlayer* getInstance();
        bool isPlaying(const char* soundFile);
    };

}
