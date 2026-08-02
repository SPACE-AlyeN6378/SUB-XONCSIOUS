#pragma once

#include <fluidsynth.h>
#include <filesystem>
#include <string>
#include <vector>

#include "track/instrument/Instrument.hpp"


struct FSInstrumentPreset
{
    int bank;
    int program;
    std::string name;
};


class FSInstrument : public Instrument
{
public:

    // Constructor
    FSInstrument();
    FSInstrument(
        const std::string &instrumentName,
        const std::filesystem::path& sf2Path,
        int blockSize_,
        double sampleRate
    );

    ~FSInstrument();

    bool load(const std::filesystem::path& sf2Path);

    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note);
    void pitchBend(int channel, int value);

    void reset() override;

    // Render a block of samples
    void render(float* left, float* right, int frames);

    // AudioSource interface
    float generate() override;


    bool isInitialized() const noexcept { return initialized; }

    std::vector<FSInstrumentPreset> getInstruments() const;

private:
    int soundFontID = -1;

    fluid_settings_t* settings = nullptr;
    fluid_synth_t* synth = nullptr;

    double sampleRate;

    std::vector<float> leftBuffer;
    std::vector<float> rightBuffer;

    size_t bufferIndex;

    bool initialized;
};
