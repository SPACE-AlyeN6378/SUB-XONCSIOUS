#include "Reverb.hpp"
#include "ReverbUtils.hpp"
#include <algorithm>
// #include <utility>
#include <cmath>
#include <stdexcept>


constexpr size_t MIN_DIFFUSION_STAGES = 4;

template <size_t NumChannels>
AudioFX::Reverb<NumChannels>::Reverb()
    : engineSampleRate(44100.0),
      meanDelayTime(200.0),
      diffusionTime(300.0),
      multiFeedback(200.0, 0.85, 44100.0)
{
}

template <size_t NumChannels>
AudioFX::Reverb<NumChannels>::Reverb(double diffusionTimeMs, double delayTimeMs, double gain, double sampleRate)
    : engineSampleRate(sampleRate),
      meanDelayTime(delayTimeMs),
      diffusionTime(diffusionTimeMs),
      multiFeedback(delayTimeMs, gain, sampleRate)
{
    generateDiffusionChain(diffusionTimeMs, sampleRate);
}

// <*complete_process>
template <size_t NumChannels>
float AudioFX::Reverb<NumChannels>::process(float input)
{
    // 1. Duplicate the input to a given number of channels
    std::array<float, NumChannels> x = ReverbUtils::split<NumChannels>(input);

    // 2. Go through each diffuser
    ArraySignal y{};
    for (auto& diffuser : diffChain)
    {
        x = diffuser.process(x);
    }

    // 3. Feedback loop
    y = multiFeedback.process(x);

    return ReverbUtils::join<NumChannels>(y);
}
// </complete_process>

template <size_t NumChannels>
std::vector<size_t> AudioFX::Reverb<NumChannels>::getFeedbackSamples()
{
    return multiFeedback.getDelaySamples();
}

template <size_t NumChannels>
void AudioFX::Reverb<NumChannels>::generateDiffusionChain(double diffusionTimeMs, double sampleRate)
{
    double d = diffusionTimeMs / (std::pow(2, MIN_DIFFUSION_STAGES) - 1);
    for (int i=0; i<MIN_DIFFUSION_STAGES; ++i)
    {
        diffChain.push_back(
            Diffusion(d, sampleRate)
        );
        d *= 2;
    }
}

template <size_t NumChannels>
std::vector<double> AudioFX::Reverb<NumChannels>::getDiffusionTimes()
{
    std::vector<double> times;
    for (Diffusion diffuser: diffChain)
    {
        times.push_back(diffuser.getMaxDelayTime());
    }

    return times;
}

template <size_t NumChannels>
void AudioFX::Reverb<NumChannels>::setDiffusionTime(double diffusionTimeMs)
{
    diffusionTime = diffusionTimeMs;
    double d = diffusionTimeMs / (std::pow(2, diffChain.size()) - 1);

    for (Diffusion& diffuser: diffChain)
    {
        diffuser.setParameters(d, engineSampleRate);
        d *= 2.0;
    }
}

template <size_t NumChannels>
void AudioFX::Reverb<NumChannels>::addDiffuser()
{
    diffChain.push_back(Diffusion());
    setDiffusionTime(diffusionTime);
}

template <size_t NumChannels>
void AudioFX::Reverb<NumChannels>::printDiffusionInfo()
{
    for (int i = 0; i < diffChain.size(); ++i)
    {
        std::cout << "DIFFUSION " << i << "\n==============================\n";
        diffChain[i].printInfo();
        std::cout << std::endl;
    }
}

template class AudioFX::Reverb<4ul>;
template class AudioFX::Reverb<8ul>;

