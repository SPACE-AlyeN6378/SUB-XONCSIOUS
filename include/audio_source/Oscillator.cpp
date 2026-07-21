#include "Oscillator.hpp"
#include <stdexcept>
#include <sstream>

Oscillator::Oscillator(OscShape shape, double frequency, float amplitude, double sampleRate)
{
    this->shape = shape;

    this->frequency.store(frequency);
    this->amplitude.store(amplitude);

    this->cache.frequency       = frequency;
    this->cache.amplitude       = amplitude;
    this->cache.phaseIncrement  = frequency / sampleRate;

    this->sampleRate = sampleRate;
}

double Oscillator::getFrequency() const noexcept
{
    return frequency.load(std::memory_order_relaxed);
}

double Oscillator::getAmplitude() const noexcept
{
    return amplitude.load(std::memory_order_relaxed);
}

double Oscillator::getPhase() const noexcept
{
    return this->phase;
}

void Oscillator::setFrequency(double f) noexcept
{
    frequency.store(f, std::memory_order_relaxed);
}

void Oscillator::setAmplitude(double a) noexcept
{
    amplitude.store(a, std::memory_order_relaxed);
}

void Oscillator::updateCache() noexcept
{
    cache.frequency         = frequency.load(std::memory_order_relaxed);
    cache.amplitude         = amplitude.load(std::memory_order_relaxed);
    cache.phaseIncrement    = cache.frequency / sampleRate;
}

float Oscillator::generate(std::size_t /*channel*/)
{
    float sample;

    switch (shape) {
        case OscShape::SINE:
            sample = cache.amplitude * shapeDef::sine(this->phase);
            break;
        case OscShape::SQUARE:
            sample = cache.amplitude * shapeDef::square(this->phase);
            break;
        case OscShape::SAWTOOTH:
            sample = cache.amplitude * shapeDef::sawtooth(this->phase);
            break;
        case OscShape::TRIANGLE:
            sample = cache.amplitude * shapeDef::triangle(this->phase);
            break;

        default: return 0.0f;
    }

    // Increment phase
    phase += cache.phaseIncrement;

    if (phase >= 1.0) phase -= 1.0;

    return sample;
}
