#ifndef MIDI_EVENT_H
#define MIDI_EVENT_H

#include <cstdint>

enum class MIDIMessage
{
    NoteOff,
    NoteOn,
    ControlChange,
    ProgramChange,
    PitchBend,
    Aftertouch
};

struct MIDIEvent
{
    MIDIMessage message;

    uint8_t channel;

    uint8_t data1; // note number, controller number, etc.
    uint8_t data2; // velocity, controller value, etc.

    uint32_t tick; // position in MIDI time

    // Operator overloading for sorting them out
    bool operator<(const MIDIEvent& other) const {

        if (this->tick != other.tick)
            return tick < other.tick;

        if (message != other.message)
            return this->message < other.message;

        return this->channel < other.channel;
    }

    
    auto operator<=>(const MIDIEvent&) const = default;
};

#endif  // MIDI_EVENT_H