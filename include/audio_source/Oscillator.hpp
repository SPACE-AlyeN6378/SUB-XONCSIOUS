#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP
#include "OscShape.hpp"
#include "AudioSource.hpp"
#include <atomic>

/* Cached oscillator parameters used by the audio thread.
 * Values are copied from atomics once per update() call to avoid
 * atomic loads during per-sample generation. 
 */
struct OscCache {
    double frequency;
    double phaseIncrement;
    float amplitude;
};

class Oscillator : public AudioSource
{
public:
    // Constructor
    Oscillator(OscShape shape, double frequency, float amplitude, double sampleRate);

    // ******** Getters ********

    double getFrequency() const noexcept;
    double getAmplitude() const noexcept;
    double getPhase() const noexcept;
    
    // ******** Setters ********

    /**
     * @brief Updates the frequency done by a user or program.
    */
    void setFrequency(double f) noexcept;

    /**
     * @brief Updates the amplitude done by a user or program.
    */
    void setAmplitude(double a) noexcept;

    /**
     * @brief Updates the oscillator’s internal cached state from shared parameters.
     *
     * This function transfers the latest control-thread values (e.g. frequency,
     * amplitude) from atomic storage into local cached variables used by the
     * real-time audio thread.
     *
     * It is designed to be called from the audio callback thread once per
     * processing block (not per sample), ensuring deterministic audio behavior
     * and avoiding expensive atomic operations inside the audio generation loop.
     *
     * Typical usage:
     * - Control thread: calls setFrequency(), setAmplitude(), etc. (thread-safe)
     * - Audio thread: calls update() once per buffer to refresh cached values
     * - Audio thread: calls generate() per sample using cached values only
     *
     * @note This function must NOT be called from multiple threads simultaneously.
     *       It is intended to run exclusively in the audio callback context.
     *
     * @warning Calling this per-sample will reduce performance and defeat the
     *          purpose of caching control values.
     */
    void updateCache() noexcept;

    float generate() override;
    void reset() override;

private:
    std::atomic<double> frequency;
    std::atomic<float> amplitude;
    double phase{0.0};
    OscShape shape;
    OscCache cache;

    // FUTURE CONSIDERATIONS:
    /*
    - Support for wave shape changing in real-time
    - Add support for frequency modulation (FM) and amplitude modulation (AM)
    */
};

#endif // OSCILLATOR_HPP