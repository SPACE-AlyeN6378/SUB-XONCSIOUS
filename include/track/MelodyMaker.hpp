#pragma once

#include "track/MIDITrack.hpp"
#include "track/instrument/Instrument.hpp"
#include "track/midi/MidiMap.h"

namespace Debug
{
inline MidiTrack makeTrack(std::shared_ptr<Instrument> inst)
{
    MidiTrack track(inst);

    track.addNote(midiMap::G3, 90, 0, 960, 0);
    track.addNote(midiMap::C4, 90, 240, 960, 0);
    track.addNote(midiMap::D4, 90, 480, 960, 0);
    track.addNote(midiMap::G4, 90, 720, 960, 0);

    track.addNote(midiMap::C4, 90, 1200, 960, 0);
    track.addNote(midiMap::D4, 90, 1440, 960, 0);
    track.addNote(midiMap::G4, 90, 1680, 960, 0);


    track.addNote(midiMap::G3, 90, 1920, 960, 0);
    track.addNote(midiMap::C4, 90, 240 + 1920, 960, 0);
    track.addNote(midiMap::D4, 90, 480 + 1920, 960, 0);
    track.addNote(midiMap::G4, 90, 720 + 1920, 960, 0);

    track.addNote(midiMap::C4, 90, 1200 + 1920, 960, 0);
    track.addNote(midiMap::D4, 90, 1440 + 1920, 960, 0);
    track.addNote(midiMap::G4, 90, 1680 + 1920, 960, 0);

    return track;
}
}