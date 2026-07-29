#ifndef NOTE_TO_MIDI_HPP
#define NOTE_TO_MIDI_HPP

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace midi {

inline int noteToMidi(const std::string& note)
{

    if(note.empty())
        return 60;

    static const std::vector<std::string> names =
    {
        "c", "c#", "d", "d#", "e", "f", "f#", "g",
        "g#", "a", "a#", "b"
    };

    std::string name;

    int octave;
    
    if(note.size() >= 2 && note[1] == '#')
    {
        name = note.substr(0,2);
        octave = std::stoi(note.substr(2));
    }

    else
    {
        name = note.substr(0,1);
        octave = std::stoi(note.substr(1));
    }



    int semitone = 0;


    for(int i=0;i<names.size();i++)
    {
        if(name == names[i])
        {
            semitone = i;
            break;
        }
    }


    // SFZ uses C1 = MIDI 24

    return (octave + 1) * 12 + semitone;
}

}  // namespace midi

#endif // NOTE_TO_MIDI_HPP