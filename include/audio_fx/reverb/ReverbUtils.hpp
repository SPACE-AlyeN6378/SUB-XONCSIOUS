#ifndef REVERB_UTILS_HPP
#define REVERB_UTILS_HPP

#include <array>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

#include "audio_fx/delay/Delay.hpp"

namespace AudioFX
{
namespace ReverbUtils
{

template <size_t NumChannels>
inline std::array<float, NumChannels> split(float input)
{
    std::array<float, NumChannels> output{};
    output.fill(input);

    return output;
}


template <size_t NumChannels>
inline float join(const std::array<float, NumChannels>& outputs)
{
    float sum = 0.0f;
    for (int i=0; i<NumChannels; ++i)
    {
        sum += outputs[i];
    }
    return sum;
}


inline constexpr size_t MIN_DELAY_SAMPLES = 10;
inline std::mt19937 rng{std::random_device{}()};

inline double meanSamples(double roomSizeMeters, double sampleRate)
{
    return roomSizeMeters * sampleRate / 516.0;
}

inline void generateBounds(
    size_t& lowerBound,
    size_t& upperBound,
    double meanDelayTimeMs,
    double sampleRate,
    double deviation = 0.4)
{
    const double meanSamplesValue = meanDelayTimeMs * sampleRate / 1000.0;
    const double delta = meanSamplesValue * deviation;

    lowerBound = std::max(
        MIN_DELAY_SAMPLES,
        static_cast<size_t>(std::floor(meanSamplesValue - delta)));

    upperBound = std::min(
        48000ul,
        static_cast<size_t>(std::ceil(meanSamplesValue + delta)));
}

template<size_t NumChannels>
inline bool isCoprime(
    size_t candidate,
    size_t index,
    const std::array<Delay<48000>, NumChannels>& delays)
{
    for (size_t i = 0; i < index; ++i)
    {
        if (std::gcd(candidate, delays[i].getDelay()) != 1)
            return false;
    }

    return true;
}

template<size_t NumChannels>
inline std::array<Delay<48000>, NumChannels> generateDelays(
    double meanDelayTimeMs,
    double sampleRate)
{
    std::array<Delay<48000>, NumChannels> delays{};

    size_t lowerBound = 0;
    size_t upperBound = 0;

    generateBounds(lowerBound, upperBound, meanDelayTimeMs, sampleRate);

    std::uniform_int_distribution<size_t> dist(lowerBound, upperBound);

    size_t index = 0;

    while (index < NumChannels)
    {
        size_t candidate = dist(rng);

        if (isCoprime<NumChannels>(candidate, index, delays))
        {
            delays[index].setDelay(candidate);
            ++index;
        }
    }

    return delays;
}

template<size_t NumChannels>
inline std::array<Delay<48000>, NumChannels> generateDelaysMax(
    double maxDelayTimeMs,
    double sampleRate)
{
    std::array<Delay<48000>, NumChannels> delays{};

    size_t lowerBound = 0;
    size_t upperBound = static_cast<size_t>(std::ceil(maxDelayTimeMs * sampleRate / 1000.0));

    std::uniform_int_distribution<size_t> dist(lowerBound, upperBound);

    size_t index = 0;
    while (index < NumChannels)
    {
        size_t candidate = dist(rng);

        if (isCoprime(candidate, index, delays))
        {
            delays[index].setDelay(candidate);
            ++index;
        }
    }

    return delays;
}


template<size_t NumChannels>
inline std::array<size_t, NumChannels> generatePermutation()
{
    std::array<size_t, NumChannels> permutation{};
    std::iota(permutation.begin(), permutation.end(), 0u);
    std::shuffle(permutation.begin(), permutation.end(), rng);
    return permutation;
}

template<size_t NumChannels>
inline std::array<float, NumChannels> generatePolarity()
{
    std::array<float, NumChannels> polarity;

    std::uniform_int_distribution<int> pdist(0, 1);
    for (int i=0; i<NumChannels; ++i)
    {
        polarity[i] = pdist(rng) ? 1.0f : -1.0f;
    }

    return polarity;
}

} // namespace ReverbUtils
} // namespace AudioFX

#endif  // REVERB_UTILS_HPP