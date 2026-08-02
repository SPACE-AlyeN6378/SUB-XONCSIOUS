#ifndef MIDI_CLIP_DAO_HPP
#define MIDI_CLIP_DAO_HPP

#include "track/midi/MusicNote.hpp"
#include "track/midi/MidiEvent.h"
#include <vector>
#include <filesystem>
#include <algorithm>

class MidiClip
{
public:

    MidiClip() = default;
    
    const std::vector<MIDIEvent>& getEvents() noexcept
    {
        if (dirty)
        {
            rebuild();
            dirty = false;
        }

        return events;
    }

    
    const std::vector<MusicNote>& retrieveNotes() const noexcept
    {
        return notes;
    }


    MusicNote getNote(size_t index) const
    {
        return notes[index];
    }


    std::size_t size() const noexcept
    {
        return notes.size();
    }

    
    void addNote(const MusicNote& note) noexcept
    {
        notes.push_back(note);
        dirty = true;
    }


    void removeNote(size_t index)
    {
        notes.erase(notes.begin() + index);
        dirty = true;
    }


    void replaceNote(size_t index, const MusicNote& note)
    {
        notes[index] = note;
        dirty = true;
    }


    void moveNote(size_t index, uint32_t x, uint8_t y)
    {
        notes[index].move(x, y);
        dirty = true;
    }


    void resizeNote(size_t index, uint32_t duration)
    {
        notes[index].duration = duration;
        dirty = true;
    }


    bool needsCleaning() const noexcept { return dirty; }


    void load();    // FUTURE CONS: Implement this function
    void save();    // FUTURE CONS: Implement this function


private:

    void rebuild()
    {
        events.clear();

        for (const auto& note: notes)
        {
            // Note On
            events.push_back(
                MIDIEvent{MIDIMessage::NoteOn, note.channel, note.pitch, note.velocity, note.startTick}
            );

            // Note off
            events.push_back(
                MIDIEvent{MIDIMessage::NoteOff, note.channel, note.pitch, note.velocity, note.endTick()}
            );
        }

        std::sort(events.begin(), events.end());
    }

    bool dirty = false;

    std::vector<MusicNote> notes;
    std::vector<MIDIEvent> events;
};


#endif  // MIDI_CLIP_DAO_HPP