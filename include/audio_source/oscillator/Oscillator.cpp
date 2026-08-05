#include "Oscillator.hpp"
#include <stdexcept>
#include <sstream>

Oscillator::Oscillator()
{
    this->shape = OscShape::SINE;

    this->frequency.store(440.0);
    this->amplitude.store(1.0f);

    this->cache.frequency       = 440.0;
    this->cache.amplitude       = 1.0f;
    this->cache.phaseIncrement  = 440.0 / 44100.0;

    this->engineSampleRate = 44100.0;
    engineSampleRate = 44100.0;
}

Oscillator::Oscillator(OscShape shape, double frequency, float amplitude, double sampleRate)
{
    this->shape = shape;

    this->frequency.store(frequency);
    this->amplitude.store(amplitude);

    this->cache.frequency       = frequency;
    this->cache.amplitude       = amplitude;
    this->cache.phaseIncrement  = frequency / sampleRate;

    this->engineSampleRate = sampleRate;
    engineSampleRate = 44100.0;
}

Oscillator::Oscillator(Oscillator&& other) noexcept
    : shape(other.shape), phase(other.phase), cache(other.cache)
{
    frequency.store(other.frequency.load(std::memory_order_relaxed), std::memory_order_relaxed);
    amplitude.store(other.amplitude.load(std::memory_order_relaxed), std::memory_order_relaxed);
    engineSampleRate = other.engineSampleRate;
}

Oscillator& Oscillator::operator=(Oscillator&& other) noexcept
{
    if (this != &other)
    {
        shape = other.shape;
        phase = other.phase;
        cache = other.cache;
        frequency.store(other.frequency.load(std::memory_order_relaxed), std::memory_order_relaxed);
        amplitude.store(other.amplitude.load(std::memory_order_relaxed), std::memory_order_relaxed);
        engineSampleRate = other.engineSampleRate;
    }

    return *this;
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

void Oscillator::setShape(OscShape sh) noexcept
{
    shape = sh;
}

void Oscillator::setPhase(double phi)
{
    if (phi < 0 || phi > 1)
        throw std::invalid_argument("Only accepts phases between 0 and 1");

    this->phase = phi;
}

void Oscillator::setSampleRate(double sampleRate_) noexcept
{
    this->cache.phaseIncrement  = frequency / sampleRate_;
    engineSampleRate = sampleRate_;
}

void Oscillator::updateCache() noexcept
{
    cache.frequency         = frequency.load(std::memory_order_relaxed);
    cache.amplitude         = amplitude.load(std::memory_order_relaxed);
    cache.phaseIncrement    = cache.frequency / engineSampleRate;
}

// <*osc_generate>
float Oscillator::generate()
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
// </osc_generate>

void Oscillator::reset()
{
    this->cache.phaseIncrement = this->cache.frequency / engineSampleRate;
    this->phase = 0.0;
}
