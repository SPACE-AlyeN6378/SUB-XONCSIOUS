#ifndef BODE_PLOT_HPP
#define BODE_PLOT_HPP

#include <cmath>
#include <fstream>
#include <vector>
#include "audio_fx/eq_filter/BiQuadFilter.hpp"
#include "audio_source/Oscillator.hpp"

using Signal = std::vector<float>;

constexpr double SAMPLE_RATE = 48000.0;
constexpr unsigned int NUMBER_OF_SAMPLES = 48000;
constexpr unsigned int WARMUP_SAMPLES = 1000;

namespace BodePlot {

double rms(const Signal& signal)
{
    double sum = 0.0;

    for (float x : signal)
        sum += static_cast<double>(x) * x;

    return std::sqrt(sum / signal.size());
}


float getGain(double inputFreq, BiQuadFilter& filter)
{
    filter.reset();

    Oscillator osc(OscShape::SINE, inputFreq, 1.0f, SAMPLE_RATE);

    // Allow filter states to settle
    for (unsigned int i = 0; i < WARMUP_SAMPLES; ++i)
        filter.process(osc.generate());


    Signal original;
    Signal filtered;

    original.reserve(NUMBER_OF_SAMPLES);
    filtered.reserve(NUMBER_OF_SAMPLES);


    for (unsigned int i = 0; i < NUMBER_OF_SAMPLES; ++i)
    {
        float sample = osc.generate();

        original.push_back(sample);
        filtered.push_back(filter.process(sample));
    }


    return static_cast<float>(
        20.0 * std::log10(rms(filtered) / rms(original))
    );
}


void plot(BiQuadFilter& filter)
{
    std::ofstream outfile("tests/frequency_response.csv");

    outfile << "Input Frequency,Gain (dB)\n";


    constexpr double startFreq = 20.0;
    constexpr double endFreq = 20000.0;
    constexpr int numberOfPoints = 500;


    for (int i = 0; i < numberOfPoints; ++i)
    {
        // Logarithmic frequency spacing
        double ratio = static_cast<double>(i) / (numberOfPoints - 1);

        double frequency = startFreq *
            std::pow(endFreq / startFreq, ratio);


        float gainDB = getGain(frequency, filter);

        outfile << frequency << "," << gainDB << "\n";
    }

    outfile.close();

    std::system("python3 tests/bode_plot.py");
}

}

#endif // BODE_PLOT_HPP