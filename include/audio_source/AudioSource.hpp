#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP
#include <numbers>


constexpr float INV_SQRT2 = 1.0f / std::numbers::sqrt2_v<float>;

class AudioSource {

public:

    virtual ~AudioSource() = default;
    virtual float generate() = 0;
    virtual void reset() = 0;

    static float downmixToMono(float left, float right)
    {
        return (left + right) * INV_SQRT2;
    }

protected:
    // unsigned long framesPerBuffer;
    double engineSampleRate;
};
    
#endif // AUDIO_SOURCE_HPP
