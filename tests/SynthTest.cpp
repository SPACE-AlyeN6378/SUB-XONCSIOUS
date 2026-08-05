#include "audio_source/oscillator/Oscillator.hpp"
#include "audio_source/audio_clip/AudioClipExporter.hpp"
#include "track/instrument/osc_synth/Unison.hpp"
#include "track/midi/MidiMap.h"
#include <vector>
#include <random>

int main()
{
    SubXoncsious::SynthUtils::Unison unison(1, midiMap::A3, OscShape::SAWTOOTH, 0.0, 0.4f, 44100.0);
    // Oscillator osc(OscShape::SAWTOOTH, 220, 0.5, 44100.0);
    std::vector<float> renderBuffer;

    unison.setVoices(3);
    unison.setDetune(10.0);
    unison.setBlend(0.3f);

    // Oscillator osc(OscShape::SINE, 440.0, 0.7f, 44100.0);

    for (int i=0; i < 44100 * 6; ++i)
    {
        renderBuffer.push_back(unison.generate());

        if (i == 22050 * 1) unison.setFrequency(midiMap::C4);
        if (i == 22050 * 2) unison.setFrequency(midiMap::D4);
        if (i == 22050 * 3) unison.setFrequency(midiMap::E4);
        if (i == 22050 * 4) unison.setFrequency(midiMap::G4);
    }

    AudioExporter::exportWav(
        "output.wav",
        renderBuffer,
        1,
        44100.0
    );


    return 0;
}