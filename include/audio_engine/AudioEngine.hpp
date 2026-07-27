#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP

#include <portaudio.h>
#include <vector>
#include <atomic>
#include <memory>
#include "track/voice/Voice.hpp"
#include "audio_source/AudioSource.hpp"
#include "audio_source/audio_clip/AudioClipPlayer.hpp"
#include "audio_fx/reverb/Reverb.hpp"
#include <cstdint>


constexpr std::size_t MAX_DELAY = 48000;

struct AudioCallbackStats
{
    std::atomic<double> callbackTimeUs{0.0};
    std::atomic<uint64_t> sumUs{0};
    std::atomic<uint64_t> count{0};
    std::atomic<uint64_t> maxUs{0};

    std::atomic<uint64_t> squareSumUs{0};
};


/**
 * @brief Real-time audio engine responsible for audio device management
 * and stream processing.
 *
 * AudioEngine initializes PortAudio, manages the audio callback,
 * and coordinates audio generation from registered audio sources.
 *
 * The audio callback must remain real-time safe and therefore performs
 * no heap allocations, file I/O, locking, or other blocking operations.
 *
 * Audio generation is delegated to AudioSource-derived objects which
 * are mixed together before being sent to the output device.
 */
class AudioEngine 
{
public:
    // Constructor
    AudioEngine();
    AudioEngine(double sampleRate_, unsigned long framesPerBuffer_);

    // Destructor
    ~AudioEngine() noexcept;

    // **************** GETTERS ****************

    /**
    * @brief Retrieves the current sample rate.
    */
    double getSampleRate() const noexcept;

    /**
    * @brief Retrieves the current frames per buffer.
    */
    unsigned long getFramesPerBuffer() const noexcept;

    /**
     * @brief Calculates and retrieves the buffer duration in milliseconds based on the sample rate and frames per buffer.
     * @return Buffer duration in milliseconds.
    */
    double getBufferDurationMs() const noexcept;

    double getCallbackTimeUs() const noexcept;
    

    // ********** STATISTICS GETTERS ***********
    double getAvgCallbackTimeUs() const noexcept;
    double getMaxCallbackTimeUs() const noexcept;
    double getStdDevCallbackTimeUs() const noexcept;


    // **************** SETTERS ****************

    /**
    * @brief Sets an audio source, which can either be an oscillator or a wave source.
    */
    void setVoice(Voice* voice_);
    // TODO: Create more setters here

    // *****************************************

    /**
     * @brief Initializes the audio subsystem (PortAudio setup).
     * 
     * Prepares PortAudio for use and validates whether the system audio
     * backend is available or not. MUST BE called before start().
     * 
     * This function does not start audio streaming.
     * 
     * @throws std::runtime_error if the initialization fails
     */
    void initialize();

    /**
     * @brief Opens and starts the audio stream.
     *
     * Creates and starts the PortAudio stream using the configured
     * sample rate, buffer size, and audio callback.
     *
     * Audio processing begins immediately after this call.
     *
     * Requires initialize() to have been called successfully.
     * 
     * @throws `std::runtime_error` if there are any hidden errors
     */
    void start();

    /**
     * @brief Stops and closes the audio stream.
     *
     * Safely stops audio processing and closes the PortAudio stream.
     * After this call, audio callback execution will cease.
     *
     * The engine can be restarted again using start().
     */
    void stop();

    /**
     * @brief Gracefully shuts down and terminates PortAudio
     */
    void shutdown();

    // TODO: Temporary function. Please remove after experimentation
    void debug();
    


    


private:
    // TODO: Change this to list voices where it contains multiple oscillators
    Voice* voice{nullptr};

    
    double sampleRate {44100.0};
    unsigned long framesPerBuffer{256};
    uint64_t sampleCounter {0};
    bool initialized{false};

    AudioClipPlayer player;
    AudioFX::Reverb<8> reverb;
    // FeedbackDelay<MAX_DELAY> fdR;

    PaStream* stream{nullptr};

    // Used to store elasped time between audio callbacks for latency measurement and debugging
    AudioCallbackStats stats;
    
    
    /**
     * @brief Real-time audio processing callback (PortAudio stream callback).
     *
     * This function is called by the PortAudio engine in a real-time audio thread
     * to generate and fill audio output buffers.
     *
     * It must execute within strict real-time constraints:
     * - No dynamic memory allocation (no new/delete/malloc/free)
     * - No blocking operations (no file I/O, locks, or system calls)
     * - No exceptions must escape the function
     * - Execution time must remain deterministic to avoid audio dropouts
     *
     * The callback processes `framesPerBuffer` audio frames and writes
     * interleaved audio samples into the provided output buffer.
     *
     * The `userData` parameter is used to pass a pointer to the AudioEngine
     * or AudioState instance, allowing access to synthesizer state,
     * voices, or audio sources.
     *
     * The return value controls stream behavior:
     * - paContinue: continue audio processing
     * - paComplete: finish playback and stop stream
     * - paAbort: immediately stop the stream due to error
     *
     * @param inputBuffer  Input audio buffer (may be nullptr if not used)
     * @param outputBuffer Output audio buffer (must be filled with audio samples)
     * @param framesPerBuffer Number of audio frames to process
     * @param timeInfo Timing information for synchronization (stream time, host time)
     * @param statusFlags Flags indicating underflow/overflow or other stream events
     * @param userData Pointer to user-defined audio state (typically this AudioEngine)
     *
     * @return paContinue to keep stream running, paComplete to stop normally,
     *         or paAbort to stop immediately due to error.
     *
     * @see https://files.portaudio.com/docs/v19-doxydocs/portaudio_8h.html#a9b8f1d7a7d3a7d5f6f8f3d9d8b8a1c2a (PaStreamCallback)
     */
    static int callback(
        const void* /*inputBuffer*/,    // TODO: Uncomment this if you're ready for mic input
        void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* /*timeInfo*/,
        PaStreamCallbackFlags /*statusFlags*/,
        void* userData
    );
};


#endif