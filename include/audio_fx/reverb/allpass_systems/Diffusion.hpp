#ifndef DIFFUSION_HPP
#define DIFFUSION_HPP

#include <cstddef>
#include "audio_fx/delay/Delay.hpp"
#include <vector>
#include "audio_fx/reverb/Hadamard.hpp"
#include <iostream>

// using DelayArray = std::array<Delay<48000>, NumChannels>;
    
namespace AudioFX::ReverbUtils
{
template<size_t NumChannels>
class Diffusion
{
public:
    // Type aliasing
    using DelayArray = std::array<Delay<48000>, NumChannels>;
    using Permutation = std::array<size_t, NumChannels>;
    using ArraySignal = std::array<float, NumChannels>;

     /**
    * @brief Default Constructor
    */
    Diffusion()
    {
        setParameters(60.0, 44100.0);
        setPermutation();
        setPolarity();
    }

    /**
    * @brief Constructor with parameters
    */
    Diffusion(double maxDelayTimeMs, double sampleRate)
    {
        setParameters(maxDelayTimeMs, sampleRate);
        setPermutation();
        setPolarity();
    }

    /**
    * @brief Sets maximum delay time and regenerates random sets of delays
    */
    void setParameters(double maxDelayTimeMs, double sampleRate)
    {
        maxDelayTime = maxDelayTimeMs;
        delays = generateDelaysMax<NumChannels>(
                    maxDelayTimeMs,
                    sampleRate);
    }

    /**
    * @brief Regenerates permutation
    */
    void setPermutation() noexcept { permutation = generatePermutation<NumChannels>(); }

    /**
    * @brief Regenerates polarity
    */
    void setPolarity() noexcept 
    {
        polarity = generatePolarity<NumChannels>();
    }


    /**
    * @brief GETTER: Retrieves an array of permutation of indices
    */
    Permutation getPermutation() const noexcept { return permutation; }

    /**
    * @brief GETTER: Retrieves an array of signs
    */
    ArraySignal getPolarity() const noexcept { return polarity; }

    /**
    * @brief GETTER: Retrieves the maximum delay time
    */
    double getMaxDelayTime() const noexcept { return maxDelayTime; }

    /**
    * @brief GETTER: Retrieves the delay times
    */
    std::vector<size_t> getDelaySamples()
    {
        std::vector<size_t> samples;
        for (size_t i = 0; i < NumChannels; ++i)
        {
            samples.push_back(delays[i].getDelay());
        }

        return samples;
    }

    /**
    * @brief Part of the process, which only performs delays and shuffle
    */
    // <*diffusion_process>
    ArraySignal delayAndShuffle(const ArraySignal& input) noexcept
    {
        float delayed;
        ArraySignal output{};

        // Process delay lines
        for (size_t i = 0; i < NumChannels; ++i)
        {
            delayed = delays[i].process(input[i]);
            output[permutation[i]] = delayed * polarity[i];
        }

        return output;
    }
    

    /**
    * @brief Complete the diffusion process
    */
    ArraySignal process(const ArraySignal& input) noexcept
    {
        return Hadamard::transform<ArraySignal>(
            delayAndShuffle(input)
        );
    }
    // </diffusion_process>

    // Resets the last output to zero
    void reset() noexcept
    {
        for (int i = 0; i < NumChannels; ++i)
        {
            delays[i].reset();
        }
    }


    void printInfo() noexcept
    {
        std::cout << "Delay Time Range: 0-" << maxDelayTime << std::endl;

        std::cout << "Delays: ";
        for (int i = 0; i<NumChannels; ++i)
        {
            std::cout << delays[i].getDelay() << " ";
        }
        std::cout << std::endl;

        std::cout << "Permutation: ";
        for (int i = 0; i<NumChannels; ++i)
        {
            std::cout << permutation[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Polarity: ";
        for (int i = 0; i<NumChannels; ++i)
        {
            std::cout << polarity[i] << " ";
        }
        std::cout << std::endl;
    }
    
private:
    DelayArray delays;
    double maxDelayTime;

    Permutation permutation;
    ArraySignal polarity;
};


}

#endif  // DIFFUSION_HPP