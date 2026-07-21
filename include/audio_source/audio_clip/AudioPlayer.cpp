#include "AudioPlayer.hpp"
#include <sndfile.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#define DEBUG false

AudioClip AudioClip::import(const std::string &filename)
{
    SF_INFO info{};

    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &info);

    // If the file fails to open
    if (!file) {
        std::cerr << sf_strerror(nullptr) << "\n";
        throw std::runtime_error("Failed to open WAV file");
    }

    if (DEBUG) {
        std::cout << "Frames: " << info.frames << "\n";
        std::cout << "Channels: " << info.channels << "\n";
        std::cout << "Format: " << info.format << "\n";
    }
    
    // Create a vector which must contain samples
    std::vector<float> samples(info.frames * info.channels);

    sf_count_t count = sf_readf_float(file, samples.data(), info.frames);

    if (DEBUG)
        std::cout << "Read samples: " << count << "\n";

    // Don't forget to close!
    sf_close(file);

    return AudioClip(
        std::move(samples), 
        static_cast<unsigned int>(info.channels), 
        static_cast<double>(info.samplerate)
    );
}

float AudioClip::getSample(std::size_t frame, std::size_t channel) const
{
    if (channel >= channels)
        throw std::out_of_range("Invalid channel");

    if (frame >= getFrameCount())
        return 0.0f;
    
    // std::cout << "Index taken: " << index * channels + channel << std::endl;    
    return samples[frame * channels + channel]; 
}

std::size_t AudioClip::getFrameCount() const noexcept
{
    return samples.size() / channels;
}


void AudioPlayer::generate(float* output)
{
    // After exceeding the last frame
    if (position + 1 >= clip.getFrameCount())
    {
        // output[0] = left channel
        // output[1] = right channel
        output[0] = 0.0f;
        output[1] = 0.0f;
        return;
    }

    if (clip.channels == 1)
    {
        float mono = interpolate(0);
        output[0] = mono;
        output[1] = mono;
    }
    else if (clip.channels == 2)
    {
        output[0] = interpolate(0);
        output[1] = interpolate(1);
    }

    position += increment;
}


void AudioPlayer::reset()
{
    position = 0.0;
}

float AudioPlayer::interpolate(std::size_t channel) const
{
    std::size_t frame = static_cast<std::size_t>(position);

    double fraction = position - frame;

    float s0 = clip.getSample(frame, channel);
    float s1 = clip.getSample(frame + 1, channel);

    return s0 + fraction*(s1 - s0);
}
