#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>

#include "audio_source/oscillator/Oscillator.hpp"
#include "audio_fx/eq_filter/StateVariableFilter.hpp"
#include "audio_fx/eq_filter/FilterTypeEnum.hpp"

using Signal = std::vector<float>;
constexpr unsigned int numberOfSamples = 5000;
constexpr double SAMPLE_RATE = 44100.0f;

float rms(const Signal& signal)
{
    float sum = 0.0f;
    for (float x: signal) 
        sum += x * x;

    return std::sqrt(sum / signal.size());
}

float getGain(double inputFreq, float cutoff, float Q, FilterType filterType)
{
    Oscillator osc(OscShape::SINE, inputFreq, 1.0f, SAMPLE_RATE);
    StateVariableFilter filter(cutoff, Q, static_cast<float>(SAMPLE_RATE));

    Signal original;
    Signal filtered;

    for (int i = 0; i < numberOfSamples; ++i)
    {
        float sample = osc.generate();

        original.push_back(sample);
        filter.process(sample);

        switch (filterType) {
            case FilterType::LowPass: filtered.push_back(filter.getLowPass()); break;
            case FilterType::BandPass: filtered.push_back(filter.getBandPass()); break;
            case FilterType::HighPass: filtered.push_back(filter.getHighPass()); break;
            default: filtered.push_back(0.0f); break;
        }
    }

    return rms(filtered) / rms(original);
}


int main() {

    float gaindB;
    // float cutoffFreq = 8000.0f;
    // float resonance = 5.0f;

    // float cutoffFreq = 8000.0f;
    // float resonance = 0.707f;

    // float cutoffFreq = 10000.0f;
    // float resonance = 0.707f;

    // float cutoffFreq = 7200.0f;
    // float resonance = 4.0f;

    float cutoffFreq = 7200.0f;
    float resonance = 0.707f;

    std::ofstream outfile("tests/hp_filter_lowQ.csv");
    outfile << "Input Frequency,Gain (dB)" << std::endl;

    for (int f = 0; f <= 20000; f += 100) {
        if (f == 0)
            gaindB = 20.0f * log10(getGain(static_cast<float>(2), cutoffFreq, resonance, FilterType::HighPass));
        else
            gaindB = 20.0f * log10(getGain(static_cast<float>(f), cutoffFreq, resonance, FilterType::HighPass));
        
        outfile << f << "," << gaindB << std::endl;
    }
    outfile.close();

    return 0;
}