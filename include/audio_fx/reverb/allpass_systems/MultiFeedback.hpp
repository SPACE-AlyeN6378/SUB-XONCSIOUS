#ifndef AUDIO_FX_REVERB_ALLPASS_SYSTEMS_MULTIFEEDBACK_HPP
#define AUDIO_FX_REVERB_ALLPASS_SYSTEMS_MULTIFEEDBACK_HPP

#include "audio_fx/reverb/ReverbUtils.hpp"
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <array>
#include <iostream>
#include "audio_fx/reverb/Hadamard.hpp"


namespace AudioFX
{
    template<size_t NumChannels>
    class MultiFeedbackDelay
    {
    public:

        using ArraySignal = std::array<float, NumChannels>;

        // Constructor
        MultiFeedbackDelay(double meanDelayTimeMs, float gain_, double sampleRate) 
        : 
        meanDelayTime(meanDelayTimeMs), 
        gain(gain_), 
        lastOutput{}
        {
            delays =
                ReverbUtils::generateDelays<NumChannels>(
                    meanDelayTimeMs,
                    sampleRate);
            // for (size_t i = 0; i < NumChannels; ++i)
            // {
            //     delays[i].setDelay(delaySamples[i].getDelay());
            // }
        }

        // Process the input from a delay
        float processDelay(float input, size_t delayNumber)
        {
            return delays[delayNumber].process(input);
        }

        ArraySignal processDelay(const ArraySignal& input)
        {
            ArraySignal output{};
            for (size_t i = 0; i < NumChannels; ++i)
                output[i] = processDelay(input[i], i);

            return output;
        }

        // Apply gain by multiplying each elements with the gain
        ArraySignal applyGain(const ArraySignal& input)
        {
            ArraySignal output{};

            for (size_t i = 0; i < input.size(); ++i)
            {
                output[i] = input[i] * gain;
            }
            return output;
        }

        // Apply gain by multiplying each elements with the gain

        // <*process_feedback>
        ArraySignal process(const ArraySignal& input) noexcept
        {
            ArraySignal output{};

            // Apply Householder matrix to previous outputs
            ArraySignal feedback = Householder::transform(lastOutput);

            // Process delay lines
            for (size_t i = 0; i < NumChannels; ++i)
            {
                output[i] = delays[i].process(input[i] + gain * feedback[i]);
            }

            lastOutput = output;
            return output;
        }   
        // <*process_feedback>


        // Sets gain value
        void setGain(float value)
        {
            if (value > 1.0)
                throw std::invalid_argument("Gain cannot be greater than 1.0");

            gain = value;
        }

        // Resets the last output to zero
        void reset() noexcept
        {
            lastOutput.fill(0.0f);

            for (int i = 0; i < NumChannels; ++i)
            {
                delays[i].reset();
            }
        }

        // Getter: Retrieves the delay times
        std::vector<size_t> getDelaySamples()
        {
            std::vector<size_t> samples;
            for (size_t i = 0; i < NumChannels; ++i)
            {
                samples.push_back(delays[i].getDelay());
            }

            return samples;
        }


    private:
        std::array<Delay<48000>, NumChannels> delays;

        double meanDelayTime;
        float gain;
        ArraySignal lastOutput{};
    };

    
}

#endif  // AUDIO_FX_REVERB_ALLPASS_SYSTEMS_MULTIFEEDBACK_HPP
