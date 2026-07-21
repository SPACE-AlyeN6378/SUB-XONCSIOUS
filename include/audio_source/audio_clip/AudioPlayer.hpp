#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP

// #include "AudioClip.hpp"
#include <vector>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <iostream>
#include "../AudioSource.hpp"

struct AudioClip
{
    std::vector<float> samples;
    unsigned int channels;
    double sampleRate;

    float getSample(std::size_t frame, std::size_t channel) const;
    std::size_t getFrameCount() const noexcept;

    static AudioClip import(const std::string& filename);
};

class AudioPlayer
{
public:
    explicit AudioPlayer(const std::string& filename, double sampleRate)
        : 
        clip(AudioClip::import(filename)),
        engineSampleRate(sampleRate),
        position(0.0)
        // FUTURE: In future version control, please support surround sound
    {
        increment = clip.sampleRate / engineSampleRate;
    }

    void generate(float* output);
    void reset();

private:
    AudioClip clip;
    double position;
    double increment;
    double engineSampleRate;

    float interpolate(std::size_t channel) const;
};
#endif  // AUDIO_PLAYER_HPP
