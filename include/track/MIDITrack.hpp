#ifndef MIDI_TRACK_HPP
#define MIDI_TRACK_HPP


#include "track/midi/MidiClipDAO.hpp"
#include "track/midi/MidiEvent.h"
#include "track/midi/MusicNote.hpp"
#include "track/instrument/Instrument.hpp"
#include <memory>
#include <cstdint>

class MidiTrack {
    
public:

    MidiTrack();
    explicit MidiTrack(std::shared_ptr<Instrument> inst);

    void togglePlayPause();

    float generateSample(uint32_t currentTick);
    void reset();

    // CRUD Operations of MIDI
    void addNote(uint8_t pitch, 
    uint8_t velocity, uint32_t start, 
    uint32_t duration, uint8_t channel = 0);
    
    // Getters
    MusicNote getNote(std::size_t noteIndex);
    std::size_t getNumberOfNotes();

private:
    MidiClip midi;
    std::shared_ptr<Instrument> instrument;

    // State variables
    bool playing;
    std::size_t eventIndex;
    std::vector<MIDIEvent> events;
};

#endif   // MIDI_TRACK_HPP