#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include "audio_source/AudioSource.hpp"
#include <string>
#include "track/midi/MidiClipDAO.hpp"

class Instrument : public AudioSource
{
public:
    Instrument() : name("Untitled Instrument"), engineSampleRate(44100.0),
                    blockSize(512)
    {
    }

    Instrument(
        const std::string& instrumentName,
        std::size_t blockSize_,
        double sampleRate
    )
        : name(instrumentName), 
        blockSize(blockSize_),
        engineSampleRate(sampleRate)
        
    { 
    }

    virtual ~Instrument() = default;

    virtual void noteOn(
        int channel, int note, int velocity
    ) = 0;


    virtual void noteOff(
        int channel, int note
    ) = 0;


    virtual void pitchBend(
        int channel, int value
    ) = 0;


    void triggerMidiEvent(const MIDIEvent& event)
    {
        switch (event.message)
        {
            case MIDIMessage::NoteOn: noteOn(event.channel, event.data1, event.data2); break;
            case MIDIMessage::NoteOff: noteOff(event.channel, event.data1); break;
            case MIDIMessage::PitchBend: pitchBend(event.channel, event.data1); break;
            default: break;
        }
    }

    void rename(const std::string& instrumentName) { name = instrumentName; }
    const std::string& getName() const { return name; }


protected:
    std::string name;
    double engineSampleRate;
    std::size_t blockSize;
};

#endif  // INSTRUMENT_HPP