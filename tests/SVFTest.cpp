#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "audio_source/Oscillator.hpp"
#include "audio_fx/eq_filter/StateVariableFilter.hpp"
#define ENABLE_PLOTTING 0

using Signal = std::vector<float>;
constexpr unsigned int numberOfSamples = 5000;
constexpr double SAMPLE_RATE = 44100.0f;

float rms(const std::vector<float>& signal)
{
    float sum = 0.0f;
    for (float x: signal) 
        sum += x * x;

    return std::sqrt(sum / signal.size());
}

TEST_CASE("Verifying Sine Wave Oscillator", "[SineWave]")
{
    Oscillator osc(OscShape::SINE, 1000.0f, 1.0f, SAMPLE_RATE);
    std::vector<float> samples;

    if (ENABLE_PLOTTING) {
        std::ofstream outfile("tests/samples.csv");
        for (int i = 0; i < 1000; ++i)
        {
            outfile << i << "," << osc.generate() << std::endl;
        }
        outfile.close();
    // REQUIRE(1 + 1 == 2);

    // SECTION("Oscillator include is available")
    // {
    //     // Replace this with real tests once the Oscillator API is known.
    //     REQUIRE(std::isfinite(0.0));
    // }
        std::system("python3 tests/basic_plot.py");
    }
    
}

TEST_CASE("SVF outputs zero for zero input")
{
    StateVariableFilter filter(1000.0f, 0.5f, static_cast<float>(SAMPLE_RATE));

    for (int i = 0; i < 1000; i++)
    {
        filter.process(0.0f);
        CHECK(filter.getHighPass() == Catch::Approx(0.0f));
        CHECK(filter.getLowPass() == Catch::Approx(0.0f));
        CHECK(filter.getBandPass() == Catch::Approx(0.0f));
    }
}

TEST_CASE("Testing Filter from SVF at Different Frequencies")
{   
    StateVariableFilter svf(1000.0f, 0.707f, static_cast<float>(SAMPLE_RATE));

    Oscillator osc(OscShape::SINE, 100.0f, 1.0f, SAMPLE_RATE);

    Signal original;
    Signal filteredLow;
    Signal filteredBand;
    Signal filteredHigh;

    SECTION("Case f < fc") {

        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        REQUIRE(gainLow == Catch::Approx(1.0f).margin(0.05f));  // Must pass through
        REQUIRE(gainBand < 0.1f);                               // Must reject
        REQUIRE(gainHigh < 0.1f);                               // Must reject
    }

    osc.setFrequency(1000.0f);
    osc.updateCache();
    original.clear();  filteredLow.clear();  filteredBand.clear();  filteredHigh.clear(); 

    SECTION("Case f = fc") {

        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        // All Must pass through with Gain = Q
        REQUIRE(gainLow == Catch::Approx(0.707f).margin(0.005f));
        REQUIRE(gainBand == Catch::Approx(0.707f).margin(0.005f));     
        REQUIRE(gainHigh == Catch::Approx(0.707f).margin(0.005f));
    }

    osc.setFrequency(10000.0f);
    osc.updateCache();
    original.clear();  filteredLow.clear();  filteredBand.clear();  filteredHigh.clear(); 
    
    SECTION("Case f > fc") {

        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        REQUIRE(gainHigh == Catch::Approx(1.0f).margin(0.05f));  // Must pass through
        REQUIRE(gainBand < 0.1f);                               // Must reject
        REQUIRE(gainLow < 0.1f);                               // Must reject
    }
}

TEST_CASE("Testing Filter from SVF at Different Resonances")
{   
    StateVariableFilter svf(1000.0f, 0.2f, static_cast<float>(SAMPLE_RATE));
    Oscillator osc(OscShape::SINE, 1000.0f, 1.0f, SAMPLE_RATE);

    Signal original;
    Signal filteredLow;
    Signal filteredBand;
    Signal filteredHigh;

    SECTION("Case Q = 0.2") {
        
        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        // All Must pass through with Gain = Q
        REQUIRE(gainLow == Catch::Approx(0.2f).margin(0.005f));
        REQUIRE(gainBand == Catch::Approx(0.2f).margin(0.005f));     
        REQUIRE(gainHigh == Catch::Approx(0.2f).margin(0.005f));
    }

    svf.setResonance(0.5f);
    osc.updateCache();
    original.clear();  filteredLow.clear();  filteredBand.clear();  filteredHigh.clear(); 

    SECTION("Case Q = 0.5") {
        
        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        // All Must pass through with Gain = Q
        REQUIRE(gainLow == Catch::Approx(0.5f).margin(0.005f));
        REQUIRE(gainBand == Catch::Approx(0.5f).margin(0.005f));     
        REQUIRE(gainHigh == Catch::Approx(0.5f).margin(0.005f));
    }

    svf.setResonance(10.0f);
    osc.updateCache();
    original.clear();  filteredLow.clear();  filteredBand.clear();  filteredHigh.clear(); 

    SECTION("Case Q = 10.0") {
        
        for (int i = 0; i < numberOfSamples; ++i)
        {
            float sample = osc.generate();

            original.push_back(sample);
            svf.process(sample);
            filteredLow.push_back(svf.getLowPass());
            filteredBand.push_back(svf.getBandPass());
            filteredHigh.push_back(svf.getHighPass());
        }

        float gainLow = rms(filteredLow) / rms(original);
        float gainBand = rms(filteredBand) / rms(original);
        float gainHigh = rms(filteredHigh) / rms(original);

        // All Must pass through with Gain = Q
        REQUIRE(gainLow == Catch::Approx(10.0f).margin(0.5f));
        REQUIRE(gainBand == Catch::Approx(10.0f).margin(0.5f));     
        REQUIRE(gainHigh == Catch::Approx(10.0f).margin(0.5f));
    }
}

TEST_CASE("Testing Frequencies at two overlapping Sine waves")
{
    StateVariableFilter svf(1000.0f, 0.707f, static_cast<float>(SAMPLE_RATE));
    Oscillator osc1(OscShape::SINE, 100.0f, 1.0f, SAMPLE_RATE);
    Oscillator osc2(OscShape::SINE, 10000.0f, 0.3f, SAMPLE_RATE);

    Signal filtered1;
    Signal filtered2;

    for (int i = 0; i < numberOfSamples; ++i)
    {
        float sample = osc1.generate() + osc2.generate();

        svf.process(sample);
        filtered1.push_back(svf.getLowPass());
        filtered2.push_back(svf.getHighPass());
    }

    float gain1 = rms(filtered1) * sqrt(2);
    float gain2 = rms(filtered2) * sqrt(2);

    CHECK(gain1 == Catch::Approx(1.0f).margin(0.005));
    CHECK(gain2 == Catch::Approx(0.3f).margin(0.005));
}




