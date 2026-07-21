#include "Voice.hpp"
#include "track/midi/MidiFreq.h"
#include <cmath>


Voice::Voice(OscShape shape, double sampleRate)
    : osc(shape, 440.0, 0.0f, sampleRate)
{   
}


void Voice::noteOn(uint8_t n, uint8_t vel)
{
    note = n;
    velocity = vel;

    osc.setFrequency(midi::frequency[n]);
    osc.setAmplitude(static_cast<float>(vel) / 127.0f);

    active = true;
}


void Voice::noteOff()
{
    active = false;
}


float Voice::generate()
{
    if (!active)
        return 0.0f;

    return osc.generate();
}


bool Voice::isActive() const noexcept
{
    return active;
}

void Voice::updateCache() noexcept
{
    osc.updateCache();
}
