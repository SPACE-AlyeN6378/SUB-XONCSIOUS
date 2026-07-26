#ifndef DELAY_HPP
#define DELAY_HPP

#include <array>
#include <cstddef>
#include <stdexcept>

template<size_t MaxDelay>
class Delay
{
public:
    // Default constructor
    Delay() : delaySamples(1000)
    {}

    // Constructor with sample given
    explicit Delay(size_t samples)
        : delaySamples(samples)
    {
        if (delaySamples == 0 || delaySamples > MaxDelay)
            throw std::invalid_argument("Invalid delay length");
    }

    //<*process_delay>
    float process(float input)
    {
        // Read delayed sample
        float output = buffer[readIndex++];

        // Write new sample
        buffer[writeIndex++] = input;

        if (writeIndex >= delaySamples)
            writeIndex = 0;

        if (readIndex >= delaySamples)
            readIndex = 0;

        return output;
    }
    //</process_delay>

    void setDelay(size_t samples)
    {
        if (samples == 0 || samples > MaxDelay)
            throw std::invalid_argument("Invalid delay length");

        delaySamples = samples;

        // Reset positions to avoid discontinuities
        writeIndex = 0;
        readIndex = 0;
    }

    void reset()
    {
        buffer.fill(0.0f);
        writeIndex = 0;
        readIndex = 0;
    }

    size_t getDelay() const
    {
        return delaySamples;
    }


private:
    std::array<float, MaxDelay> buffer{};
    size_t writeIndex = 0;
    size_t readIndex = 0;
    size_t delaySamples;
};


#endif  // REVERB_HPP