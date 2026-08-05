#include "FSInstrument.hpp"

// #include <algorithm>
// #include <cstring>

FSInstrument::FSInstrument()
:
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
    sampleRate(sampleRate)
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

    readIndex = 0;

    std::fill(leftBuffer.begin(), leftBuffer.end(), 0.0f);
    std::fill(rightBuffer.begin(), rightBuffer.end(), 0.0f);
}

// <*fs_render>
void FSInstrument::render()
{
    if (!synth)
    {
        std::fill(leftBuffer.begin(), leftBuffer.end(), 0.0f);
        std::fill(rightBuffer.begin(), rightBuffer.end(), 0.0f);
        readIndex = 0;
        return;
    }

    fluid_synth_write_float(
        synth,
        static_cast<int>(blockSize),
        leftBuffer.data(),
        0,
        1,
        rightBuffer.data(),
        0,
        1
    );

    readIndex = 0;
}

float FSInstrument::generate()
{
    if (readIndex >= blockSize)
    {
        render();
    }

    // AudioSource only returns one float,
    // so downmix to mono for now
    float sample = downmixToMono(
        leftBuffer[readIndex],
        rightBuffer[readIndex]
    );

    ++readIndex;

    return sample * 2.0f;
}
// </fs_render>

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
