#include "AudioEngine.hpp"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <alsa/asoundlib.h>
#include "track/midi/MidiMap.h"
#include <string>
// #include <memory>



// const std::string WAV_DIR{"include/track/metronome/dry-wood-block.wav"};
const std::string WAV_DIR{"/home/space_alyen/Downloads/Windows_XP.wav"};
// const std::string WAV_DIR{
//     "sonatina/Samples/Grand Piano/piano-p-c5.wav"
// };

// constexpr size_t delaySamples = 3200;
constexpr float GAIN = 0.7;

AudioEngine::AudioEngine()
    : sampleRate(44100.0), framesPerBuffer(256), sampleCounter(0),
    player(WAV_DIR, 44100), reverb(300.0, 200.0, GAIN, 44100.0),
    instrument(
        "FluidR3_GM",
        "/home/space_alyen/Music/Libraries/Soundfonts/FluidR3 GM + GS.sf2",
        512,
        44100
    )
{
    debug();
}

AudioEngine::AudioEngine(double sr, unsigned long frames)
    : sampleRate(sr), framesPerBuffer(frames), sampleCounter(0),
    player(WAV_DIR, 44100), reverb(300.0, 200.0, GAIN, 44100.0),
    instrument(
        "FluidR3_GM",
        "/home/space_alyen/Music/Libraries/Soundfonts/FluidR3 GM + GS.sf2",
        512,
        44100
    )
{
    debug();
}

AudioEngine::~AudioEngine() noexcept
{
    try
    {
        stop();
        shutdown();
    }
    catch (...)
    {
        // Intentionally ignored.
    }
}

double AudioEngine::getSampleRate() const noexcept
{
    return sampleRate;
}

unsigned long AudioEngine::getFramesPerBuffer() const noexcept
{
    return framesPerBuffer;
}

double AudioEngine::getBufferDurationMs() const noexcept
{
    return (static_cast<double>(framesPerBuffer) / sampleRate) * 1000.0;
}

double AudioEngine::getAvgCallbackTimeUs() const noexcept
{
    return stats.sumUs.load() / (double)stats.count.load();
}

double AudioEngine::getMaxCallbackTimeUs() const noexcept
{
    return stats.maxUs.load();
}

double AudioEngine::getStdDevCallbackTimeUs() const noexcept
{
    double n = (double)stats.count.load();

    if (n <= 1.0) // Avoid division by zero or negative variance
        return 0.0;

    double sum = stats.sumUs.load();
    double squareSum = stats.squareSumUs.load();

    // Standard deviation based on the sample formula: sqrt((Σ(xi - μ)²) / N - 1)
    double variance = (squareSum - sum * sum / n) / (n - 1.0);
    return std::sqrt(variance);
}

void AudioEngine::setVoice(Voice *voice_)
{
    this->voice = voice_;
}

void AudioEngine::initialize()
{
    fflush(stderr);

    int oldErr = dup(STDERR_FILENO);

    FILE* null = fopen("/dev/null", "w");
    dup2(fileno(null), STDERR_FILENO);

    PaError err = Pa_Initialize();

    fflush(stderr);

    dup2(oldErr, STDERR_FILENO);
    close(oldErr);
    fclose(null);

    if (err != paNoError)
        throw std::runtime_error(Pa_GetErrorText(err));

    initialized = true;
}

void AudioEngine::start()
{
    if (!initialized)
        throw std::runtime_error("Audio engine must be initialized before start()");

    if (Pa_GetDeviceCount() <= 0 || Pa_GetDefaultOutputDevice() == paNoDevice)
    {
        std::cout << "No audio output device available; skipping audio stream start." << std::endl;
        return;
    }

    PaError err;

    err = Pa_OpenDefaultStream(
        &stream,
        0, // input channels
        2, // output channels
        paFloat32,
        sampleRate,
        framesPerBuffer,
        callback,
        this);

    if (err != paNoError)
    {
        if (err == paDeviceUnavailable || err == paInvalidDevice)
        {
            std::cout << "Audio device is unavailable; skipping audio stream start." << std::endl;
            return;
        }

        throw std::runtime_error(Pa_GetErrorText(err));
    }

    // Start the stream
    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        if (err == paDeviceUnavailable || err == paInvalidDevice)
        {
            std::cout << "Audio device is unavailable; skipping audio stream start." << std::endl;
            return;
        }

        throw std::runtime_error(Pa_GetErrorText(err));
    }
}

void AudioEngine::stop()
{
    if (stream == nullptr)
        return;

    PaError err;

    int active = Pa_IsStreamActive(stream);
    if (active < 0)
        throw std::runtime_error(Pa_GetErrorText(active));

    if (active == 1)
    {
        err = Pa_StopStream(stream);
        if (err != paNoError)
            throw std::runtime_error(Pa_GetErrorText(err));
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        throw std::runtime_error(Pa_GetErrorText(err));

    stream = nullptr;
}

void AudioEngine::shutdown()
{
    if (!initialized)
        return;

    PaError err = Pa_Terminate();

    if (err != paNoError)
        throw std::runtime_error(Pa_GetErrorText(err));

    initialized = false;
}

void AudioEngine::debug()
{
    instrument.setInstrument(0, 0, 25);
    notes.push_back(midiMap::C4);
    notes.push_back(midiMap::D4);
    notes.push_back(midiMap::E4);
    notes.push_back(midiMap::F4);
    notes.push_back(midiMap::G4);
    notes.push_back(midiMap::A4);
    notes.push_back(midiMap::B4);
    notes.push_back(midiMap::C5);
}

int AudioEngine::callback(
    const void * /* inputBuffer */,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *userData)
{
    AudioEngine *state = static_cast<AudioEngine *>(userData);
    float *out = static_cast<float *>(outputBuffer);

    if (state == nullptr)
        return paAbort;

    if (state->voice == nullptr)
        return paAbort;

    // Updates the wavetable's internal cached state from shared parameters (frequency, amplitude, etc.)
    // state->voice->updateCache();

    // Start measuring the time taken for this callback
    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned long i = 0; i < framesPerBuffer; ++i)
    {
        // Every second
        if (state->sampleCounter % 44100 == 0)
        {
            if (state->noteIndex > 0)
            {
                int previous =
                    state->notes[(state->noteIndex - 1 + state->notes.size())
                                % state->notes.size()];

                state->instrument.noteOff(0, previous);
            }

            // Play next note
            int note = state->notes[state->noteIndex];
            state->instrument.noteOn(0, note, 100);

            state->noteIndex =
                (state->noteIndex + 1) % state->notes.size();
        }
        


        float audio = state->instrument.generate();

        out[i * 2]     = audio;
        out[i * 2 + 1] = audio;

        ++state->sampleCounter;
    }

    // Finish measuring the time taken for this callback
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // ========= Statistical accumulation for callback time measurement =========
    // accumulate sum
    state->stats.sumUs.fetch_add(elapsedUs, std::memory_order_relaxed);
    state->stats.count.fetch_add(1, std::memory_order_relaxed);

    // accumulate square sum
    state->stats.squareSumUs.fetch_add(elapsedUs * elapsedUs, std::memory_order_relaxed);

    // track max
    uint64_t prevMax = state->stats.maxUs.load(std::memory_order_relaxed);
    while (elapsedUs > prevMax &&
           !state->stats.maxUs.compare_exchange_weak(prevMax, elapsedUs))
    {
    }

    // ==========================================================================

    return paContinue;
}
