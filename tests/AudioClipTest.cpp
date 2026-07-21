#include <iostream>
#include "audio_source/audio_clip/AudioPlayer.hpp"
#include <vector>

int main() {
    
    const char* home = std::getenv("HOME");
    std::string path = "/home/space_alyen/Downloads/o95.wav";

    try {
        std::cout << "Import path: " << path << std::endl;
        // std::cout << "Channels: " << clip.getChannels() << std::endl;
        // std::cout << "Sampling rate: " << clip.getSampleRate() << std::endl;
        // std::cout << "Audio Size: " << clip.getAudioSize() << std::endl;

        // std::cout << "Max value: " << clip.getMaxGain() << std::endl;
        // std::cout << "Min value: " << clip.getMinGain() << std::endl;

        AudioPlayer player(path, 44100.0);

        for (int i = 0; i < 5000; ++i)
            std::cout << player.generate(0) << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error importing audio: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}