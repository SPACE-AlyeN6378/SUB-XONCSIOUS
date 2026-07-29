#ifndef SFZ_INSTRUMENT_HPP
#define SFZ_INSTRUMENT_HPP

#include <string>
#include <vector>

#include <sfizz.h>

#include "track/instrument/Instrument.hpp"


class SFZInstrument : public Instrument
{
public:

    SFZInstrument();

    SFZInstrument(
        const std::string& instrumentName,
        const std::string& sfzPath,
        int blockSize,
        double sampleRate
    );

    // Cleanup
    ~SFZInstrument() override;


    // Audio generation
    float generate() override;

    // Reset synth
    void reset() override;


    // MIDI interface
    void noteOn(
        int channel,
        int note,
        int velocity
    ) override;

    void noteOff(int channel, int note) override;

    void pitchBend(int channel, int value) override;

    // Render audio block
    void render();


    bool isLoaded() const { return loaded; };


private:

    sfizz_synth_t* synth;

    bool loaded;

    // Output buffers
    std::vector<float> leftBuffer;
    std::vector<float> rightBuffer;


    // Current read position
    int readIndex;
};

#endif  // SFZ_INSTRUMENT_HPP