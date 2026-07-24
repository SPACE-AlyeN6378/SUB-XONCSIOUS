#include "FeedbackDelay.hpp"
#include <stdexcept>

template <size_t MaxDelay>
FeedbackDelay<MaxDelay>::FeedbackDelay(size_t delaySamples, float gain_)
    : delay(delaySamples), gain(gain_)
{
    if (gain_ > 1.0)
        throw std::invalid_argument("Gain cannot be greater than 1.0");
}

template <size_t MaxDelay>
float FeedbackDelay<MaxDelay>::process(float input) noexcept
{
    float delayed = delay.process(input + gain * lastOutput);
    lastOutput = delayed;
    return delayed;
}


template <size_t MaxDelay>
void FeedbackDelay<MaxDelay>::setGain(float value)
{
    if (value > 1.0)
        throw std::invalid_argument("Gain cannot be greater than 1.0");

    gain = value;
}

template <size_t MaxDelay>
void FeedbackDelay<MaxDelay>::reset() noexcept
{
    delay.reset();
    lastOutput = 0.0f;
}

template class FeedbackDelay<48000ul>;
