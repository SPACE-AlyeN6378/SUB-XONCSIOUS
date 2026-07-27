#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP

// #include "AudioClip.hpp"
#include <vector>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <numbers>
#include "../AudioSource.hpp"


struct AudioClip
{
    std::vector<float> samples;
    double sampleRate;

    float getSample(std::size_t frame) const;
    std::size_t getFrameCount() const noexcept;

    static AudioClip import(const std::string& filename);
};

class AudioClipPlayer
{
public:
    explicit AudioClipPlayer(const std::string& filename, double sampleRate)
        : 
        clip(AudioClip::import(filename)),
        engineSampleRate(sampleRate),
        position(0.0)

    {
        increment = clip.sampleRate / engineSampleRate;
    }

    float generate();
    void reset();

private:
    AudioClip clip;
    double position;
    double increment;
    double engineSampleRate;

    float interpolate() const;
};
#endif  // AUDIO_PLAYER_HPP
