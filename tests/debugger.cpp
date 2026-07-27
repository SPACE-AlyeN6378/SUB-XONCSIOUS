#include <iostream>
#include <numbers>
#include "audio_source/audio_clip/AudioClipPlayer.hpp"

constexpr float INV_SQRT2 = 1.0f / std::numbers::sqrt2_v<float>;

int main()
{
    AudioClipPlayer player("/home/space_alyen/Downloads/o95.wav", 44100.0);

    for (int i=0; i<44100 * 10; ++i)
    {
        float output = player.generate();
        // std::cout << output << std::endl;
        if (output > 1e-6)
        {   
            std::cout << output << std::endl;
        }
    }
    return 0;
}