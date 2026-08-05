#include <catch2/catch_test_macros.hpp>
#include <algorithm>

#include "track/midi/MidiClipDAO.hpp"
#include "track/midi/MusicNote.hpp"
#include "track/MelodyMaker.hpp"   // Your header containing Debug::makeTrack


TEST_CASE("MidiClip can add and retrieve notes", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note;

    note.pitch = 60;
    note.velocity = 100;
    note.channel = 0;
    note.startTick = 480;
    note.duration = 240;

    clip.addNote(note);

    const auto& notes = clip.retrieveNotes();

    REQUIRE(notes.size() == 1);

    REQUIRE(notes[0].pitch == 60);
    REQUIRE(notes[0].velocity == 100);
    REQUIRE(notes[0].channel == 0);
    REQUIRE(notes[0].startTick == 480);
    REQUIRE(notes[0].duration == 240);
}


TEST_CASE("MidiClip can retrieve note by index", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note;

    note.pitch = 64;
    note.velocity = 80;
    note.channel = 1;
    note.startTick = 960;
    note.duration = 480;

    clip.addNote(note);

    MusicNote retrieved = clip.getNote(0);

    REQUIRE(retrieved.pitch == 64);
    REQUIRE(retrieved.velocity == 80);
    REQUIRE(retrieved.channel == 1);
    REQUIRE(retrieved.startTick == 960);
    REQUIRE(retrieved.duration == 480);
}


TEST_CASE("MidiClip marks events dirty after adding note", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note;

    note.pitch = 60;
    note.velocity = 100;
    note.channel = 0;
    note.startTick = 0;
    note.duration = 480;

    clip.addNote(note);

    const auto& events = clip.getEvents();

    REQUIRE(events.size() == 2);

    REQUIRE(events[0].message == MIDIMessage::NoteOn);
    REQUIRE(events[1].message == MIDIMessage::NoteOff);
}


TEST_CASE("MidiClip removes notes", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note1;
    note1.pitch = 60;

    MusicNote note2;
    note2.pitch = 62;

    clip.addNote(note1);
    clip.addNote(note2);

    REQUIRE(clip.retrieveNotes().size() == 2);

    clip.removeNote(0);

    REQUIRE(clip.retrieveNotes().size() == 1);
    REQUIRE(clip.retrieveNotes()[0].pitch == 62);
}


TEST_CASE("MidiClip replaces notes", "[MidiClip]")
{
    MidiClip clip;

    MusicNote oldNote;
    oldNote.pitch = 60;

    MusicNote newNote;
    newNote.pitch = 72;

    clip.addNote(oldNote);

    clip.replaceNote(0, newNote);

    REQUIRE(clip.getNote(0).pitch == 72);
}


TEST_CASE("MidiClip moves notes", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note;

    note.pitch = 60;
    note.startTick = 100;

    clip.addNote(note);

    clip.moveNote(0, 500, 64);

    auto moved = clip.getNote(0);

    REQUIRE(moved.startTick == 500);
    REQUIRE(moved.pitch == 64);
}


TEST_CASE("MidiClip resizes notes", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note;

    note.pitch = 60;
    note.duration = 100;

    clip.addNote(note);

    clip.resizeNote(0, 800);

    REQUIRE(clip.getNote(0).duration == 800);
}


TEST_CASE("MidiClip generates sorted MIDI events", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note1;
    note1.pitch = 60;
    note1.velocity = 100;
    note1.channel = 0;
    note1.startTick = 960;
    note1.duration = 240;


    MusicNote note2;
    note2.pitch = 64;
    note2.velocity = 100;
    note2.channel = 0;
    note2.startTick = 120;
    note2.duration = 480;


    MusicNote note3;
    note3.pitch = 67;
    note3.velocity = 100;
    note3.channel = 0;
    note3.startTick = 480;
    note3.duration = 120;


    // Add them out of order intentionally
    clip.addNote(note1);
    clip.addNote(note2);
    clip.addNote(note3);

    REQUIRE(clip.size() == 3);
    const auto& events = clip.getEvents();


    REQUIRE(events.size() == 6); // 3 notes * (NoteOn + NoteOff)


    REQUIRE(std::is_sorted(
        events.begin(),
        events.end()
    ));
}


TEST_CASE("MidiClip sorts events with identical ticks", "[MidiClip]")
{
    MidiClip clip;

    MusicNote note1;
    note1.pitch = 60;
    note1.startTick = 480;
    note1.duration = 480;


    MusicNote note2;
    note2.pitch = 62;
    note2.startTick = 480;
    note2.duration = 240;


    clip.addNote(note1);
    clip.addNote(note2);


    const auto& events = clip.getEvents();


    REQUIRE(std::is_sorted(
        events.begin(),
        events.end()
    ));
}


#include <catch2/catch_test_macros.hpp>


#include "track/instrument/Instrument.hpp"
#include "track/midi/MusicNote.hpp"


TEST_CASE("Debug MIDI track contains correct notes", "[MidiTrack][Debug]")
{
    // Instrument is not needed for testing notes.
    // Replace nullptr with a mock instrument if your constructor requires it.
    MidiTrack track = Debug::makeTrack(nullptr);


    SECTION("Track contains correct number of notes")
    {
        REQUIRE(track.getNumberOfNotes() == 14);
    }


    SECTION("First melody phrase is correct")
    {
        auto note0 = track.getNote(0);
        auto note1 = track.getNote(1);
        auto note2 = track.getNote(2);
        auto note3 = track.getNote(3);


        REQUIRE(note0.pitch == midiMap::G3);
        REQUIRE(note0.velocity == 90);
        REQUIRE(note0.startTick == 0);
        REQUIRE(note0.duration == 960);
        REQUIRE(note0.channel == 0);


        REQUIRE(note1.pitch == midiMap::C4);
        REQUIRE(note1.startTick == 240);


        REQUIRE(note2.pitch == midiMap::D4);
        REQUIRE(note2.startTick == 480);


        REQUIRE(note3.pitch == midiMap::G4);
        REQUIRE(note3.startTick == 720);
    }


    SECTION("Second phrase starts after one bar")
    {
        auto note = track.getNote(7);

        REQUIRE(note.pitch == midiMap::G3);
        REQUIRE(note.startTick == 1920);
        REQUIRE(note.duration == 960);
    }


    SECTION("All notes have expected velocity and channel")
    {
        for (size_t i = 0; i < 14; ++i)
        {
            auto note = track.getNote(i);

            REQUIRE(note.velocity == 90);
            REQUIRE(note.channel == 0);
            REQUIRE(note.duration == 960);
        }
    }
}