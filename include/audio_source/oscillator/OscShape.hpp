#ifndef OSCSHAPE_HPP
#define OSCSHAPE_HPP

#include <cmath>
#include <numbers>

namespace shapeDef
{
    inline float sine(float phase)
    {
        constexpr float tau = 2.0f * std::numbers::pi_v<float>;
        return static_cast<float>(std::sin(tau * phase));
    }

    inline float square(float phase)
    {
        return (phase < 0.5f) ? 1.0f : -1.0f;
    }

    inline float sawtooth(float phase)
    {
        return 2.0f * phase - 1.0f; // Linear ramp from -1 to 1
    }

    inline float triangle(float phase)
    {
        return 1.0f - 4.0f * std::abs(phase - 0.5f);
    }
}

enum class OscShape {
    SINE,
    SQUARE,
    SAWTOOTH,
    TRIANGLE
};

#endif // OSCSHAPE_HPP