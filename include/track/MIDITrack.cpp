#include "MIDITrack.hpp"


MidiTrack::MidiTrack() :
    midi(), instrument(nullptr),
    playing(false), eventIndex(0)
{
}

MidiTrack::MidiTrack(std::shared_ptr<Instrument> inst) :
    midi(), instrument(std::move(inst)),
    playing(false), eventIndex(0)
{
}


void MidiTrack::togglePlayPause()
{
    playing = !playing;
}


float MidiTrack::generateSample(uint32_t currentTick)
{
    // Check if the MIDI events need to be refreshed or filled
    if (events.empty() || midi.needsCleaning())
    {
        events = midi.getEvents();
    }


    float sample;
    if (!playing || eventIndex >= events.size() || !instrument) 
    { sample = 0.0f; }

    else {
        while (eventIndex < events.size() &&
                events[eventIndex].tick == currentTick)
        {
            instrument->triggerMidiEvent(events[eventIndex]);
            ++eventIndex;
        }

        sample = instrument->generate();
    }

    return sample;  // Audio effects will later be applied to the track
}


void MidiTrack::reset()
{
    if (instrument) instrument->reset();

    playing = false;
    eventIndex = 0;
    events = midi.getEvents();
}

void MidiTrack::addNote(uint8_t pitch, 
    uint8_t velocity, uint32_t start, 
    uint32_t duration, uint8_t channel)
{
    MusicNote note;
    note.pitch = pitch;
    note.velocity = velocity;
    note.startTick = start;
    note.duration = duration;
    note.channel = channel;
    midi.addNote(note);
}

MusicNote MidiTrack::getNote(std::size_t noteIndex)
{
    return midi.getNote(noteIndex);
}

std::size_t MidiTrack::getNumberOfNotes()
{
    return midi.retrieveNotes().size();
}
