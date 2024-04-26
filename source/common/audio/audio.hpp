//
// Created by HIMA on 4/26/2024.
//

#include <string>
namespace our {
    class AudioPlayer {
    public:
        AudioPlayer();

        ~AudioPlayer();
        bool load(const std::string& filePath);
        bool load(const std::string& filePath, int volumeLevel);
        void play(bool loop = false);

        void stop();

        bool isPlaying() const;
        bool setVolume(int volumeLevel);

    private:
        bool m_isPlaying;
        int m_volumeLevel;
    };

}
