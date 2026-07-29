#include "FSInstrument.hpp"

// #include <algorithm>
// #include <cstring>

FSInstrument::FSInstrument()
:
    leftBuffer(512, 0.0f),
    rightBuffer(512, 0.0f),
    bufferIndex(0),
    initialized(false)
{
    settings = new_fluid_settings();

    fluid_settings_setnum(
        settings,
        "synth.sample-rate",
        sampleRate
    );

    synth = new_fluid_synth(settings);
}


FSInstrument::FSInstrument(
    const std::string &instrumentName,
    const std::filesystem::path& sf2Path,
    int blockSize_,
    double sampleRate
)
    :
    Instrument(instrumentName, blockSize_, sampleRate), 
    sampleRate(sampleRate),
    leftBuffer(blockSize_, 0.0f),
    rightBuffer(blockSize_, 0.0f)
{
    settings = new_fluid_settings();

    fluid_settings_setnum(settings, "synth.sample-rate", sampleRate);

    synth = new_fluid_synth(settings);

    if (!load(sf2Path))
    {
        throw std::runtime_error(
            "Failed to load SoundFont: " + sf2Path.string()
        );
    }

    initialized = true;
}


bool FSInstrument::load(const std::filesystem::path& sf2Path)
{
    if (!synth)
        return false;

    soundFontID = fluid_synth_sfload(synth, sf2Path.c_str(), 1);

    return soundFontID >= 0;
}


FSInstrument::~FSInstrument()
{
    if (synth)
        delete_fluid_synth(synth);

    if (settings)
        delete_fluid_settings(settings);
}


void FSInstrument::noteOn(int channel, int key, int velocity)
{
    if (!synth) return;

    fluid_synth_noteon(synth, channel, key, velocity);
}


void FSInstrument::noteOff(int channel, int key)
{
    if (!synth)
        return;

    fluid_synth_noteoff(synth, channel, key);
}


void FSInstrument::pitchBend(int channel, int value)
{
    if (!synth) return;

    fluid_synth_pitch_bend(synth, channel, value);
}


void FSInstrument::reset()
{
    if (!synth)
        return;

    fluid_synth_system_reset(synth);

    bufferIndex = 0;

    std::fill(leftBuffer.begin(), leftBuffer.end(), 0.0f);
    std::fill(rightBuffer.begin(), rightBuffer.end(), 0.0f);
}


void FSInstrument::render(float* left, float* right, int frames)
{
    if (!synth)
    {
        std::fill(left, left + frames, 0.0f);
        std::fill(right, right + frames, 0.0f);
        return;
    }

    fluid_synth_write_float(synth, frames, left, 0, 1, right, 0, 1);
}


float FSInstrument::generate()
{
    if (bufferIndex >= blockSize)
    {
        render(
            leftBuffer.data(), rightBuffer.data(), blockSize
        );

        bufferIndex = 0;
    }

    // AudioSource only returns one float,
    // so downmix to mono for now
    float sample = downmixToMono(
        leftBuffer[bufferIndex],
        rightBuffer[bufferIndex]
    );

    ++bufferIndex;

    return sample * 2.0f;
}


bool FSInstrument::setInstrument(
    int channel,
    int bank,
    int program
)
{
    if (!synth || soundFontID < 0)
        return false;


    return fluid_synth_program_select(
        synth,
        channel,
        soundFontID,
        bank,
        program
    ) == FLUID_OK;
}

std::vector<FSInstrumentPreset> FSInstrument::getInstruments() const
{
    std::vector<FSInstrumentPreset> presets;

    if (!synth || soundFontID < 0)
        return presets;

    fluid_sfont_t* sfont =
        fluid_synth_get_sfont_by_id(synth, soundFontID);

    if (!sfont)
        return presets;

    fluid_preset_t* preset;

    fluid_sfont_iteration_start(sfont);

    while ((preset = fluid_sfont_iteration_next(sfont)) != nullptr)
    {
        presets.push_back({
            fluid_preset_get_banknum(preset),
            fluid_preset_get_num(preset),
            fluid_preset_get_name(preset)
        });
    }

    return presets;
}
