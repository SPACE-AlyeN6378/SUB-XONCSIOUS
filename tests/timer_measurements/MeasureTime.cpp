#include "misc/Timer.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <random>
#include <functional>
#include <cstdint>
#include "track/instrument/fluidsynth/FSInstrument.hpp"
#include "track/instrument/sfz/SFZInstrument.hpp"
#include "track/instrument/osc_synth/Unison.hpp"
#include "track/MIDITrack.hpp"
#include "track/midi/MidiMap.h"
#include "audio_source/oscillator/Oscillator.hpp"
#include "track/MelodyMaker.hpp"

#include "audio_fx/eq_filter/StateVariableFilter.hpp"
#include "audio_fx/eq_filter/BiQuadFilter.hpp"
#include "audio_fx/compressor/Compressor.hpp"
#include "audio_fx/reverb/Reverb.hpp"
#include "audio_fx/delay/FeedbackDelay.hpp"

#include "audio_source/audio_clip/AudioClipExporter.hpp"
#include "audio_source/audio_clip/AudioClipPlayer.hpp"


constexpr std::size_t BUFFER_SIZE = 512;
constexpr std::size_t MEASUREMENTS = 1005;
constexpr double SAMPLE_RATE = 44100.0;


std::vector<float> generateWhiteNoise(std::size_t size)
{
    std::vector<float> noise(size);
    static std::mt19937 generator(std::random_device{}());

    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    for (float& sample : noise)
    {
        sample = distribution(generator);
    }

    return noise;
}


void measureTime(std::function<float()> func)
{
    std::vector<double> measurements;

    for (int i = 0; i < MEASUREMENTS; ++i)
    {
        TestUtils::Timer timer(measurements);

        for (int j = 0; j < BUFFER_SIZE; ++j)
        {
            float v = func(); (void)v;
        }
    }

    for (double elasped: measurements)
    {
        std::cout << elasped << std::endl;
    }
}




int main()
{
    // AudioClipPlayer player("/home/space_alyen/Downloads/o95.wav", SAMPLE_RATE);
    AudioClipPlayer player("include/track/metronome/dry-wood-block.wav", SAMPLE_RATE);
    FeedbackDelay<48000> delay(11025, 0.6);
    
    std::array<float, BUFFER_SIZE> output{};
    std::vector<double> measurements;
    std::vector<float> white_noise = generateWhiteNoise(BUFFER_SIZE);
        

    generateWhiteNoise(BUFFER_SIZE);

    for (int i = 0; i < MEASUREMENTS; ++i)
    {
        TestUtils::Timer timer(measurements);

        for (int j = 0; j < BUFFER_SIZE; ++j)
        {
            output[j] = delay.process(white_noise[i]);
        }
    }

    for (double elasped: measurements)
    {
        std::cout << elasped << std::endl;
    }

    // EXPORT AND LISTEN TO THE AUDIO SAMPLE HERE
    // {
    //     std::vector<float> renderBuffer;
    //     std::vector<float> white_noise = generateWhiteNoise(SAMPLE_RATE * 5);

    //     for (int i = 0; i < SAMPLE_RATE * 8; ++i)
    //     {
    //         renderBuffer.push_back(delay.process(player.generate()));
    //     }

    //     AudioExporter::exportWav("tests/output.wav", renderBuffer, 1, 44100.0);
    // }
    
    return 0;
    
}