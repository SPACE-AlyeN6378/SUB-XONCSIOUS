#include "SFZParser.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "track/midi/NoteToMidi.hpp"

std::string trim(std::string s)
{
    const char* ws = " \t\r\n";

    s.erase(0, s.find_first_not_of(ws));
    s.erase(s.find_last_not_of(ws) + 1);

    return s;
}


bool SFZParser::load(const std::string& filename)
{

    std::ifstream file(filename);


    if(!file.is_open())
    {
        std::cerr << "Cannot open SFZ file\n";
        return false;
    }

    enum class Section
    {
        NONE,
        GROUP,
        REGION
    };


    Section section = Section::NONE;

    SFZGroup currentGroup;
    SFZRegion currentRegion;

    bool inRegion = false;
    std::string line;


    while(std::getline(file, line))
    {
        // Trim out all the trailing spaces
        line = trim(line);

        if(line.empty())
            continue;
        
        // Remove comments
        auto comment = line.find("//");

        if(comment != std::string::npos)
            line.erase(comment);

        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));

        if(line.empty())
            continue;

        // Header
        if(line == "<group>")
        {
            currentGroup = SFZGroup();
            section = Section::GROUP;
            continue;
        }


        if(line == "<region>")
        {
            if(!currentRegion.sample.empty())
            {
                regions.push_back(currentRegion);
            }


            currentRegion = SFZRegion();

            // inherit group
            currentRegion.group = currentGroup;

            inRegion = true;
            section = Section::REGION;

            continue;
        }

        
        // Opcode parsing
        auto equals = line.find('=');

        if(equals == std::string::npos)
            continue;

        std::string opcode =
            line.substr(0, equals);

        std::string value =
            line.substr(equals + 1);

        opcode = trim(opcode);
        value = trim(value);

        if(section == Section::GROUP)
        {
            parseGroupOpcode(
                currentGroup,
                opcode,
                value
            );
        }


        else if(section == Section::REGION)
        {
            parseRegionOpcode(
                currentRegion,
                opcode,
                value
            );
        }

    }

    // Save last region
    if (inRegion && !currentRegion.sample.empty())
    {
        regions.push_back(currentRegion);
    }


    return true;
}


void SFZParser::parseRegionOpcode(
    SFZRegion& region,
    const std::string& opcode,
    const std::string& value)
{
    if(opcode == "sample")
    {
        region.sample = value;
    }

    else if(opcode == "lokey")
    {
        region.lowKey = midi::noteToMidi(value);
    }


    else if(opcode == "hikey")
    {
        region.highKey = midi::noteToMidi(value);
    }


    else if(opcode == "pitch_keycenter")
    {
        region.pitchKeycenter = midi::noteToMidi(value);
    }


    else if(opcode == "lovel")
    {
        region.lowVelocity = std::stoi(value);
    }


    else if(opcode == "hivel")
    {
        region.highVelocity = std::stoi(value);
    }


    else if(opcode == "ampeg_release")
    {
        region.release = std::stof(value);
    }

}


void SFZParser::parseGroupOpcode(
    SFZGroup& group,
    const std::string& opcode,
    const std::string& value)
{

    if(opcode == "lovel") { group.lowVelocity = std::stoi(value); }

    else if(opcode == "hivel") { group.highVelocity = std::stoi(value); }


    // Amplitude envelope

    else if(opcode == "ampeg_attack")
    {
        group.attack = std::stof(value);
    }

    else if(opcode == "ampeg_hold")
    {
        group.hold = std::stof(value);
    }

    else if(opcode == "ampeg_decay")
    {
        group.decay = std::stof(value);
    }

    else if(opcode == "ampeg_sustain")
    {
        group.sustain = std::stof(value);
    }

    else if(opcode == "ampeg_release")
    {
        group.release = std::stof(value);
    }


    // Velocity modulation

    else if(opcode == "ampeg_vel2attack")
    {
        group.velAttack = std::stof(value);
    }

    else if(opcode == "ampeg_vel2decay")
    {
        group.velDecay = std::stof(value);
    }


    // Filter

    else if(opcode == "fil_veltrack")
    {
        group.filterVelTrack = std::stof(value);
    }

    else if(opcode == "fil_type")
    {
        group.filterType = value;
    }

    else if(opcode == "cutoff")
    {
        group.cutoff = std::stof(value);
    }


    // Pitch

    else if(opcode == "pitch_veltrack")
    {
        group.pitchVelTrack = std::stof(value);
    }

    else if(opcode == "tune")
    {
        group.tune = std::stof(value);
    }


    // Looping
    else if(opcode == "loop_mode")
    {
        group.loopMode = value;
    }


    // Round robin
    else if(opcode == "seq_length")
    {
        group.sequenceLength = std::stoi(value);
    }

    else if(opcode == "seq_position")
    {
        group.sequencePosition = std::stoi(value);
    }

    else
    {
        // Unsupported opcode
        // Optional:
        std::cout << "Unknown group opcode: " << opcode << "\n";
    }
}
