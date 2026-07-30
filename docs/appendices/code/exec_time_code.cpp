int AudioEngine::callback(
    const void * /* inputBuffer */,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *,
    PaStreamCallbackFlags,
    void *userData)
{
    // Start measuring the time taken for this callback
    auto start = std::chrono::high_resolution_clock::now();

    // ==========================
    // Audio processing goes here
    // ==========================

    // Example:
    // - Read MIDI events
    // - Generate FluidSynth audio
    // - Mix tracks/instruments
    // - Write samples to outputBuffer

    // Finish measuring the time taken for this callback
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate elapsed time in microseconds
    auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // ========= Statistical accumulation for callback time measurement =========
    // accumulate sum
    state->stats.sumUs.fetch_add(elapsedUs, std::memory_order_relaxed);
    state->stats.count.fetch_add(1, std::memory_order_relaxed);

    // accumulate square sum
    state->stats.squareSumUs.fetch_add(elapsedUs * elapsedUs, std::memory_order_relaxed);

    // track max
    uint64_t prevMax = state->stats.maxUs.load(std::memory_order_relaxed);
    while (elapsedUs > prevMax &&
           !state->stats.maxUs.compare_exchange_weak(prevMax, elapsedUs))
    {
    }

    // ==========================================================

    return paContinue;
}