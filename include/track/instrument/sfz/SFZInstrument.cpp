#include "SFZInstrument.hpp"

SFZInstrument::SFZInstrument()
:
    synth(nullptr),
    loaded(false)
{
}

SFZInstrument::SFZInstrument(
    const std::string &instrumentName, 
    const std::string &sfzPath, 
    int blockSize_,
    double sampleRate
)
:
    Instrument(instrumentName, blockSize_, sampleRate),
    synth(nullptr),
    loaded(false)
{
    synth = sfizz_create_synth();
    if (!synth) return;

    sfizz_set_sample_rate(synth, sampleRate);
    sfizz_set_samples_per_block(synth, blockSize);
    loaded = sfizz_load_file(synth, sfzPath.c_str());
}


SFZInstrument::~SFZInstrument()
{
    if (synth)  // Prevents nullptr
        sfizz_free(synth);
}


void SFZInstrument::noteOn(int channel, int note, int velocity)
{
    sfizz_send_note_on(synth, 0, note, velocity);
}

void SFZInstrument::noteOff(int channel, int note)
{
    sfizz_send_note_off(synth, 0, note, 0);
}

void SFZInstrument::pitchBend(int channel, int value)
{
    sfizz_send_pitch_wheel(synth, 0, value);
}

// <*sfz_render>
void SFZInstrument::render()
{
    float* outputs[2] = {
        leftBuffer.data(), rightBuffer.data()
    };

    sfizz_render_block(
        synth,
        outputs,
        2,
        blockSize
    );

    readIndex = 0;
}


float SFZInstrument::generate()
{
    if (!loaded)
        return 0.0f;

    if (readIndex >= blockSize)
        render();

    float sample = downmixToMono(
        leftBuffer[readIndex],
        rightBuffer[readIndex]
    );

    ++readIndex;

    return sample;
}
// </sfz_render>

void SFZInstrument::reset()
{
    sfizz_all_sound_off(synth);

    std::fill(leftBuffer.begin(), leftBuffer.end(), 0.0f);
    std::fill(rightBuffer.begin(), rightBuffer.end(), 0.0f);

    readIndex = 0;
}