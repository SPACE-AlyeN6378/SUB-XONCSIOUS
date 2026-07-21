#include <cmath>

#include <numbers>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <exception>

#include "track/voice/Voice.hpp"
#include "audio_engine/AudioEngine.hpp"
#include "track/midi/MidiFreq.h"
#include "track/midi/MidiMap.h"


int main() {

    try {
        AudioEngine engine(44100.0, 512);
        double SAMPLE_RATE = engine.getSampleRate();

        Voice voice(OscShape::SINE, SAMPLE_RATE);
        engine.setVoice(&voice);

        // Step 1: Initialize PortAudio
        engine.initialize();

        // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Play for 1 second
        engine.start();

        std::cout << "Press Enter to stop the audio engine..." << std::endl;
        std::cin.get();  // Wait for user to press Enter

        double avgUs = engine.getAvgCallbackTimeUs();
        double avgMs = avgUs / 1000.0;
        double maxUs = engine.getMaxCallbackTimeUs();
        double maxMs = maxUs / 1000.0;
        double stdDevUs = engine.getStdDevCallbackTimeUs();
        double stdDevMs = stdDevUs / 1000.0;

        std::cout << std::fixed << std::setprecision(6);

        std::cout << "Buffer duration: " << engine.getBufferDurationMs() << " ms\n";
        std::cout << "Average callback time: "
                << avgMs << " ms\n";
        std::cout << "Maximum callback time: "
                << maxMs << " ms\n";
        std::cout << "Standard deviation: "
                << stdDevMs << " ms\n";

        PaDeviceIndex device = Pa_GetDefaultOutputDevice();

const PaDeviceInfo* info = Pa_GetDeviceInfo(device);

std::cout << "Device: "
          << info->name
          << "\nMax output channels: "
          << info->maxOutputChannels
          << "\nDefault sample rate: "
          << info->defaultSampleRate
          << '\n';

        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "AudioEngine error: " << ex.what() << std::endl;
        return 1;
    }


}
