#ifndef SFZ_PARSER_HPP
#define SFZ_PARSER_HPP

#include <string>
#include <vector>


struct SFZGroup
{
    // Velocity
    int lowVelocity = 0;
    int highVelocity = 127;


    // Envelope
    float attack = 0.0f;
    float hold = 0.0f;
    float decay = 0.0f;
    float sustain = 100.0f;
    float release = 0.0f;

    float velAttack = 0.0f;
    float velDecay = 0.0f;


    // Filter
    float cutoff = 20000.0f;
    float filterVelTrack = 0.0f;
    std::string filterType;


    // Pitch
    float tune = 0.0f;
    float pitchVelTrack = 0.0f;


    // Loop
    std::string loopMode;


    // Round robin
    int sequenceLength = 1;
    int sequencePosition = 1;
};


struct SFZRegion
{
    std::string sample;

    int lowKey = 0;
    int highKey = 127;

    int pitchKeycenter = 60;

    int lowVelocity = 0;
    int highVelocity = 127;

    float release = 0.0f;

    SFZGroup group;
};


class SFZParser
{

public:

    bool load(const std::string& filename);

    const std::vector<SFZRegion>& getRegions() const
    {
        return regions;
    }


private:

    std::vector<SFZRegion> regions;
    bool inRegion = false;

    void parseGroupOpcode(
        SFZGroup& group,
        const std::string& opcode,
        const std::string& value);

    void parseRegionOpcode(
        SFZRegion& region,
        const std::string& opcode,
        const std::string& value);
};

#endif  // SFZ_PARSER_HPP