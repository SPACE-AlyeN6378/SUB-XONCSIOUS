#ifndef MUSIC_NOTE_HPP
#define MUSIC_NOTE_HPP

#include <cstdint>

struct MusicNote
{
    uint8_t channel;
    
    uint8_t pitch;
    uint8_t velocity;

    uint32_t startTick;     // In PPQN (Pulse per quarter note)
    uint32_t duration;      // In PPQN

    uint32_t endTick() const
    {
        return startTick + duration;
    }

    void move(uint32_t x, uint8_t y)
    {
        startTick = x;
        pitch = y;
    }
};

#endif  // MUSIC_NOTE_HPP