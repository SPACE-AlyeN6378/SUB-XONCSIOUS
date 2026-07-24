#ifndef REVERB_HPP
#define REVERB_HPP

#include <cstddef>
#include <array>
#include <vector>
#include "allpass_systems/MultiFeedback.hpp"
#include "allpass_systems/Diffusion.hpp"




namespace AudioFX
{
    template <size_t NumChannels>
    class Reverb
    {
    public:
        // Type aliasing
        using ArraySignal = std::array<float, NumChannels>;
        using Diffusion = AudioFX::ReverbUtils::Diffusion<NumChannels>;
        using DiffusionList = std::vector<Diffusion>;
        using IntVector = std::vector<size_t>;


        // Default constructor
        Reverb();

        // Constructor
        Reverb(double diffusionTimeMs, double delayTimeMs, double gain, double sampleRate);

        float process(float input);

        // Getters
        std::vector<size_t> getFeedbackSamples();
        std::vector<double> getDiffusionTimes();

        /*
        * SETTER: Changes total diffusion time
        */
        void setDiffusionTime(double diffusionTime);
        void addDiffuser();

        // Printers
        void printDiffusionInfo();

        
        
    private:
        double engineSampleRate;
        double meanDelayTime;
        double diffusionTime;

        MultiFeedbackDelay<NumChannels> multiFeedback;
        DiffusionList diffChain;
        float gainLimiter = 1.0f / sqrt(NumChannels);

        // Helper functions to generate delays
        void generateDiffusionChain(double diffusionTimeMs, double sampleRate);
    };
}

template class Delay<44100ul>;
template class Delay<48000ul>;

#endif  // REVERB_HPP


