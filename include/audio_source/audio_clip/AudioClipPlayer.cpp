#include "AudioClipPlayer.hpp"
#include <sndfile.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <numbers>
#define DEBUG false


constexpr float INV_SQRT2 = 1.0f / std::numbers::sqrt2_v<float>;

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

    // Add guard for unsupported audio channels
    if (info.channels > 2)
    {
        throw std::runtime_error("Only stereo and mono channels currently supported");
    }
    
    // Create a vector which must contain samples
    std::vector<float> samples(info.frames * info.channels);

    sf_count_t count = sf_readf_float(file, samples.data(), info.frames);

    if (DEBUG)
        std::cout << "Read samples: " << count << "\n";

    // Don't forget to close!
    sf_close(file);

    // Downmix to mono
    if (info.channels == 2)
    {
        std::cerr << "WARNING: Stereo WAV detected. Downmixing to mono." << std::endl;

        std::vector<float> mono(info.frames);
        for (int i=0; i<info.frames; ++i)
        {
            mono[i] = (samples[i*2] + samples[i*2+1]) * INV_SQRT2;
        }

        samples = std::move(mono);
    }

    return AudioClip(
        std::move(samples), 
        static_cast<double>(info.samplerate)
    );
}

float AudioClip::getSample(std::size_t frame) const
{
    if (frame >= getFrameCount())
        return 0.0f;
    
    // std::cout << "Index taken: " << index * channels + channel << std::endl;    
    return samples[frame]; 
}

std::size_t AudioClip::getFrameCount() const noexcept
{
    return samples.size();
}


float AudioClipPlayer::generate()
{
    // After exceeding the last frame
    if (position + 1 >= clip.getFrameCount())
    {
        return 0.0f;
    }

    float output = interpolate();
    position += increment;

    return output;
}


void AudioClipPlayer::reset()
{
    position = 0.0;
}

float AudioClipPlayer::interpolate() const
{
    std::size_t frame = static_cast<std::size_t>(position);

    double fraction = position - frame;

    float s0 = clip.getSample(frame);
    float s1 = clip.getSample(frame + 1);

    return s0 + fraction*(s1 - s0);
}

