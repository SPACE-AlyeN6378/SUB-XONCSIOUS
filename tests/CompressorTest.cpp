#include "audio_fx/compressor/Compressor.hpp"
#include "audio_source/Oscillator.hpp"
#include <fstream>
#include <iostream>

int main()
{
    Oscillator osc(OscShape::SINE, 20.0, 0.2, 44100.0);
    // Compressor comp(-6.0f, 4.0f, 0.01f, 10.0f, 44100.0);
    Compressor comp(-6.0f, 4.0f, 200.0f, 100.0f, 10.0f, 44100.0);

    float squareSumInputRMS = 0.0f;
    float squareSumOutputRMS = 0.0f;
    unsigned long rmsCount = 0;

    std::ofstream outfile("tests/compressor_plot.csv");
    outfile << "Time,Input,Output,Gain,RMS,GainDB,RMSDB\n";

    for (unsigned long i = 0; i < 44100 * 3; ++i) // 2 seconds of audio
    {
        if (i == 44100 * 1) {
            // After 1 second, increase amplitude
            osc.setAmplitude(0.8);
            osc.updateCache();  // Update the cached amplitude value
        }

        if (i == 44100 * 2) {
            // After 2 seconds, decrease amplitude
            osc.setAmplitude(0.2);
            osc.updateCache();  // Update the cached amplitude value
        }

        float sample = osc.generate();
        float output = comp.process(sample);
        float gain = comp.getGain();
        float rms = comp.getRMS();
        float gainDb = comp.getGain(true);
        float rmsDb = comp.getRMS(true);


        if (i == 44100 * 0.2) {
            std::cout << "Gain at 0.2s: " << gain << ", Gain dB: " << gainDb << ", RMS: " << rms << ", RMS dB: " << rmsDb << std::endl;
        }


        if (i == 44100 * 1.7) {
            std::cout << "Gain at 1.7s: " << gain << ", Gain dB: " << gainDb << ", RMS: " << rms << ", RMS dB: " << rmsDb << std::endl;
        }

        // Calculating the RMS of both the input and output signals for comparison
        if (44100 * 1 < i && i < 44100 * 2)
        {
            squareSumInputRMS += sample * sample;
            squareSumOutputRMS += output * output;
            ++rmsCount;
        }

        
        double time = static_cast<double>(i) / 44100.0;
        outfile << time << "," << sample << "," << output 
        << "," << gain << "," << rms << "," << gainDb << "," << rmsDb << "\n";
        // Process the sample with the compressor here if needed
    }
    
    outfile.close();

    float RMSInputDB = 10 * log10(squareSumInputRMS / rmsCount);
    float RMSOutputDB = 10 * log10(squareSumOutputRMS / rmsCount);

    std::cout << "RMS Input (dB): " << RMSInputDB << std::endl;
    std::cout << "RMS Output (dB): " << RMSOutputDB << std::endl;

    std::system("python3 tests/compressor_plot.py"); // Call the Python script to plot the data
    return 0;
}