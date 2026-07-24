#ifndef FEEDBACK_DELAY_HPP
#define FEEDBACK_DELAY_HPP
#include <cstddef>
#include "audio_fx/delay/Delay.hpp"

template<size_t MaxDelay>
class FeedbackDelay
{
public:
    // Constructor
    FeedbackDelay(size_t delaySamples, float gain_);

    // Processes the input into output
    float process(float input) noexcept;

    // Sets gain value
    void setGain(float value);

    void reset() noexcept;


private:
    Delay<MaxDelay> delay;

    float gain;
    float lastOutput = 0.0f;
};

#endif  // FEEDBACK_DELAY_HPP
