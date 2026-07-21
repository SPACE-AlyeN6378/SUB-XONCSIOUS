#ifndef VOICE_HPP
#define VOICE_HPP

#include "audio_source/Oscillator.hpp"
#include <cstdint>

/**
 * @brief Represents a single playable note instance in the audio engine.
 *
 * A Voice is responsible for producing audio for exactly one note.
 * It manages the oscillator(s), envelope state, and note lifecycle
 * (active → release → inactive).
 *
 * Voices are not created per frame. Instead, they are reused by the
 * VoiceManager to avoid real-time allocations.
 */
class Voice
{
public:
    explicit Voice(OscShape shape, double sampleRate);

    /**
     * @brief Starts the voice for a given MIDI note.
     * @param note MIDI note number (0–127)
     * @param velocity Note velocity (0–127)
     */
    void noteOn(uint8_t n, uint8_t vel);

    /**
     * @brief Stops playback of the current note.
     */
    void noteOff();

    /**
     * @brief Generates one audio sample for this voice.
     * @return Audio sample (mono)
     *
     * Returns 0.0f if the voice is inactive.
     */
    float generate();

    /**
     * @brief Returns whether the voice is currently active or releasing.
     */
    bool isActive() const noexcept;

    // /**
    //  * @brief Returns true if the voice has fully finished and can be reused.
    //  */
    // bool isFinished() const noexcept;

    /**
     * @brief Updates the oscillator’s internal cached state from shared parameters.
     */
    void updateCache() noexcept;

private:
    Oscillator osc;

    uint8_t note{0};
    uint8_t velocity{0};

    bool active{false};

    // Placeholder for later ADSR implementation
    // bool releasing{false};   // For envelopes
    // float envelope{0.0f};    // TODO: (FUTURE CONSIDERATION) Implement the envelope
};

#endif
