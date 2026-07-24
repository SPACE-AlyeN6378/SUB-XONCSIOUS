#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <vector>
#include "audio_fx/reverb/Reverb.hpp"
#include "audio_fx/reverb/ReverbUtils.hpp"
#include "audio_fx/reverb/Reverb.hpp"
#include "audio_fx/delay/Delay.hpp"
#include "audio_fx/reverb/Hadamard.hpp"
#include "audio_fx/reverb/allpass_systems/MultiFeedback.hpp"
#include "audio_fx/reverb/allpass_systems/Diffusion.hpp"
#include <fstream>
#include <array>
#include <algorithm>

constexpr double SAMPLE_RATE = 44100;
constexpr size_t NUM_SAMPLES = 8;

bool pairwiseCoprime(const std::vector<size_t>& nums)
{
    for (size_t i = 0; i < nums.size(); ++i)
    {
        for (size_t j = i + 1; j < nums.size(); ++j)
        {
            if (std::gcd(nums[i], nums[j]) != 1)
                return false;
        }
    }

    return true;
}

template<typename T, size_t n>
void printArray(const std::array<T, n>& arr)
{
    for (size_t i = 0; i < n; ++i)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}


template<typename T>
void printVector(const std::vector<T>& vec)
{
    for (T e: vec)
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}


TEST_CASE("Reverb utility test")
{
    double mean = AudioFX::ReverbUtils::meanSamples(25.0, 44100.0);

    size_t samplesLB1, samplesUB1, samplesLB2, samplesUB2;
    size_t samLB, samUB;

    AudioFX::ReverbUtils::generateBounds(samplesLB1, samplesUB1, 200.0, SAMPLE_RATE, 0.2);
    AudioFX::ReverbUtils::generateBounds(samplesLB2, samplesUB2, 200.0, SAMPLE_RATE, 0.5);
    AudioFX::ReverbUtils::generateBounds(samLB, samUB, 200.0, SAMPLE_RATE);

    std::array<Delay<48000>, NUM_SAMPLES> delays 
    = AudioFX::ReverbUtils::generateDelays<NUM_SAMPLES>(200.0, SAMPLE_RATE);


    SECTION("Bounds")
    {
        CHECK(samplesLB1 == 7056);
        CHECK(samplesLB2 == 4410);
        CHECK(samplesUB1 == 10584);
        CHECK(samplesUB2 == 13230);


        REQUIRE(samplesLB2 <= samLB); REQUIRE(samLB <= samplesLB1);
        REQUIRE(samplesUB1 <= samUB); REQUIRE(samUB <= samplesUB2);
    }

    SECTION("Generated set of samples")
    {
        std::vector<size_t> delaySamples;
        std::cout << "Sets of delay time values: " << std::endl;
        for (auto delay: delays)
        {
            std::cout << delay.getDelay() << " ";
            delaySamples.push_back(delay.getDelay());
        }
        std::cout << "\n" << std::endl;
        
        REQUIRE(pairwiseCoprime(delaySamples));
    }
}

TEST_CASE("Reverb splits signal")
{
    auto result = AudioFX::ReverbUtils::split<4>(3.14159f);

    REQUIRE(result[0] == 3.14159f);
    REQUIRE(result[1] == 3.14159f);
    REQUIRE(result[2] == 3.14159f);
    REQUIRE(result[3] == 3.14159f);
}

TEST_CASE("Reverb joins signal")
{
    std::array<float, 4> signals = {5.0f, 6.0f, 7.0f, 8.0f};

    float result = AudioFX::ReverbUtils::join<4>(signals);
    // auto result = AudioFX::Reverb<4>::split(3.14159f);

    REQUIRE(result == 26.0f);
}


TEST_CASE("Feedback Delay Check")
{
    constexpr size_t NUM_CHANNELS = 8;
    using ArraySignal = std::array<float, NUM_CHANNELS>;

    AudioFX::MultiFeedbackDelay<NUM_CHANNELS> multiFD = AudioFX::MultiFeedbackDelay<NUM_CHANNELS>(200.0, 0.7, 44100.0);
    std::vector<size_t> samples = multiFD.getDelaySamples();

    size_t max_displacement = *std::max_element(samples.begin(), samples.end());

    SECTION("Check delays and reset")
    {
        // Check delays
        for (int t = 0; t <= max_displacement + 1; ++t)
        {
            float impulse = (t == 0) ? 1.0f : 0.0f;
            ArraySignal x = AudioFX::ReverbUtils::split<NUM_CHANNELS>(impulse);

            ArraySignal output = multiFD.processDelay(x);
            
            for (int ch = 0; ch < NUM_CHANNELS; ++ch)
            {   
                float expected = (t == samples[ch]) ? 1.0f : 0.0f;
                CHECK(output[ch] == expected);
            }
        }
    }

    multiFD.reset();
    SECTION("Apply gain to a signal")
    {
        ArraySignal x{7.0f, 2.0f, 5.0f, 5.0f, 5.0f, 6.0f, 4.0f, 6.0f};
        ArraySignal y = multiFD.applyGain(x);
        ArraySignal y_exp{4.9f, 1.4f, 3.5f, 3.5f, 3.5f, 4.2f, 2.8f, 4.2f};

        for (size_t i = 0; i < NUM_CHANNELS; ++i)
            CHECK(y == y_exp);
    }
}


TEST_CASE("Permutation test")
{
    SECTION("Display the values")
    {
        auto p = AudioFX::ReverbUtils::generatePermutation<8>();
        
        std::cout << "Permutation: " << std::endl;
        for (int i = 0; i < 8; ++i)
            std::cout << p[i] << " ";
        
        std::cout << "\n" << std::endl;
    }

    constexpr size_t NUM_CHANNELS = 4;

    SECTION("Permutation contains all indices")
    {  
        auto p = AudioFX::ReverbUtils::generatePermutation<NUM_CHANNELS>();
        std::array<bool, NUM_CHANNELS> found{};

        for (auto index: p)
        {
            REQUIRE(index < NUM_CHANNELS);
            found[index] = true;
        }

        for (auto value: found)
        {
            REQUIRE(value == true);
        }
    }

    SECTION("Permutation reorders values without modifying them")
    {
        std::array<float, NUM_CHANNELS> input = {1.0f, 2.0f, 3.0f, 4.0f};
        auto p = std::array<size_t, NUM_CHANNELS>{2, 0, 3, 1};

        std::array<float, NUM_CHANNELS> output{};

        for (size_t i = 0; i < 4; ++i)
        {
            output[i] = input[p[i]];
        }

        REQUIRE(output[0] == 3.0f);
        REQUIRE(output[1] == 1.0f);
        REQUIRE(output[2] == 4.0f);
        REQUIRE(output[3] == 2.0f);
    }
    
    SECTION("Permutation preserves energy")
    {
        std::array<float, 4> input{0.2f, -0.5f, 1.0f, 0.7f};
        auto p = AudioFX::ReverbUtils::generatePermutation<NUM_CHANNELS>();

        std::array<float, 4> output{};

        for (size_t i = 0; i < 4; ++i)
            output[i] = input[p[i]];

        float inputEnergy = 0;
        float outputEnergy = 0;

        for (size_t i = 0; i < 4; ++i)
        {
            inputEnergy += input[i] * input[i];
            outputEnergy += output[i] * output[i];
        }

        REQUIRE(outputEnergy == inputEnergy);
    }
}


TEST_CASE("Hadamard Matrix test")
{
    using ArraySignal4 = std::array<float, 4>;
    using ArraySignal8 = std::array<float, 8>;

    SECTION("Checking for vector size 4")
    {
        ArraySignal4 x = {3,1,7,9};
        ArraySignal4 y = Hadamard::transform4(x);
        ArraySignal4 expY = {10.0f, 0.0f, -6.0f, 2.0f};

        for (int i=0; i<4; ++i)
        {
            CHECK(y[i] == expY[i]);
        }
    }

    SECTION("Checking for vector size 8")
    {
        ArraySignal8 x = {1,2,3,4,5,6,7,8};
        ArraySignal8 y = Hadamard::transform8(x);
        ArraySignal8 expY = {
            12.72792f,
            -1.41421f,
            -2.82843f,
            0.0f,
            -5.65685f,
            0.0f,0.0f,0.0f
        };

        for (int i=0; i<4; ++i)
        {
            REQUIRE(y[i] == Catch::Approx(expY[i]).margin(1e-5));
        }
    }

    SECTION("Orthogonality Test")
    {
        ArraySignal8 x = {
            3.064630f,
            -2.422070f,
            9.859364f,
            3.527914f,
            9.199232f,
            -4.659123f,
            6.802913f,
            1.786096f
        };

        ArraySignal8 y = Hadamard::transform8(x);
        ArraySignal8 z = Hadamard::transform8(y);

        for (size_t i = 0; i<8; ++i)
        {
            REQUIRE(z[i] == Catch::Approx(x[i]).margin(1e-5));
        }
   }
}


TEST_CASE("Householder Matrix Test")
{
    using ArraySignal8 = std::array<float, 8>;
    using ArraySignal4 = std::array<float, 4>;

    ArraySignal8 x = {2, 5, 3, 0, 1, 6, 4, 7};
    ArraySignal8 y = Householder::transform<ArraySignal8>(x);
    ArraySignal8 expY = {-5.0f,-2.0f,-4.0f,-7.0f,-6.0f,-1.0f,-3.0f,0.0f};

    for (int i=0; i<4; ++i)
    {
        CHECK(y[i] == expY[i]);
    }

    ArraySignal4 x1 = {0.5f, 0.246f, 1.3f, 3.57f};
    ArraySignal4 y1 = Householder::transform<ArraySignal4>(x1);
    ArraySignal4 expY1 = {-2.308f, -2.562f, -1.508f,  0.762f};

    for (int i=0; i<4; ++i)
    {
        CHECK(y1[i] == Catch::Approx(expY1[i]));
    }
}


TEST_CASE("Diffusion Test")
{
    using AudioFX::ReverbUtils::Diffusion;
    constexpr size_t NUM_CHANNELS = 8;
    using IntArray = std::array<size_t, NUM_CHANNELS>;
    using ArraySignal = std::array<float, NUM_CHANNELS>;

    Diffusion<NUM_CHANNELS> diffusion(60.0, 44100.0);

    // Permutation and Polarity
    IntArray permutation = diffusion.getPermutation();
    ArraySignal polarity = diffusion.getPolarity();

    // Displacement samples
    std::vector<size_t> samples = diffusion.getDelaySamples();

    SECTION("Display Permutation and Polarity")
    {
        std::cout << "DIFF TEST 1: Display Permutations" << std::endl;
        printArray<size_t, NUM_CHANNELS>(permutation);
        std::cout << "\n";

        std::cout << "DIFF TEST 2: Polarity Generation" << std::endl;
        printArray<float, NUM_CHANNELS>(polarity);
        std::cout << "\n";
    }

    SECTION("Delay & Permutation Test")
    {
        size_t max_displacement = *std::max_element(samples.begin(), samples.end());

        // Check delays
        for (int t = 0; t <= max_displacement + 1; ++t)
        {
            float impulse = (t == 0) ? 1.0f : 0.0f;
            ArraySignal x = AudioFX::ReverbUtils::split<NUM_CHANNELS>(impulse);

            ArraySignal output = diffusion.delayAndShuffle(x);
            
            for (int ch = 0; ch < NUM_CHANNELS; ++ch)
            {   
                float expected = (t == samples[ch]) ? 1.0f : 0.0f;
                CHECK(output[permutation[ch]] * polarity[ch] == expected);
            }
        }
    }

    SECTION("Diffusion Conservation of Energy")
    {
        diffusion.reset();

        double time = 0.0;

        size_t numSamples = 44100 * 6; // 10 seconds

        double energy = 0.0;

        for (size_t t = 0; t < numSamples; ++t)
        {
            float impulse = (t == 0) ? 1.0f : 0.0f; 
            ArraySignal x = AudioFX::ReverbUtils::split<NUM_CHANNELS>(impulse);
            float output;

            ArraySignal y = diffusion.process(x);
            output = AudioFX::ReverbUtils::join<NUM_CHANNELS>(y);


            for (size_t ch = 0; ch < NUM_CHANNELS; ++ch)
            {
                energy += y[ch] * y[ch];
            }
        }

        CHECK(energy == Catch::Approx(NUM_CHANNELS));
    }
}

TEST_CASE("Diffusion Time Tuning")
{
    // AudioFX::Reverb<4> reverb(200.0, 0.7, 44100.0);
    constexpr size_t NUM_CHANNELS = 8;
    constexpr size_t DIFF_STAGES = 4;
    using ArraySignal = std::array<float, NUM_CHANNELS>;
    using Reverb = AudioFX::Reverb<NUM_CHANNELS>;

    Reverb reverb = Reverb(300.0, 200.0, 0.7, 44100.0);

    std::vector<double> diffTimes = reverb.getDiffusionTimes();
    double expected[4] = {20.0, 40.0, 80.0, 160};
    for (int i=0; i<diffTimes.size(); ++i)
    {
        CHECK(expected[i] == diffTimes[i]);
    }

    reverb.setDiffusionTime(1488.0);
    diffTimes = reverb.getDiffusionTimes();
    CHECK(diffTimes[0] == 99.2);
    CHECK(diffTimes[1] == 99.2*2);
    CHECK(diffTimes[2] == 99.2*2*2);
    CHECK(diffTimes[3] == 99.2*2*2*2);

    reverb.addDiffuser();
    diffTimes = reverb.getDiffusionTimes();
    CHECK(diffTimes[0] == 48.0);
    CHECK(diffTimes[1] == 96.0);
    CHECK(diffTimes[2] == 192.0);
    CHECK(diffTimes[3] == 384.0);
    CHECK(diffTimes[4] == 768.0);
}

TEST_CASE("Impulse Response Plot")
{
    // AudioFX::Reverb<4> reverb(200.0, 0.7, 44100.0);
    constexpr size_t NUM_CHANNELS = 8;
    constexpr size_t DIFF_STAGES = 4;
    using ArraySignal = std::array<float, NUM_CHANNELS>;
    using Reverb = AudioFX::Reverb<NUM_CHANNELS>;

    Reverb reverb = Reverb(300.0, 200.0, 0.8, 44100.0);

    reverb.printDiffusionInfo();
    
    // reverb.addDiffuser();
    printVector<double>(reverb.getDiffusionTimes());

    std::ofstream outfile("tests/impulse_response.csv");

    outfile << "time,amplitude\n";

    double time = 0.0;

    size_t numSamples = 44100 * 10; // 10 seconds
    float max_value = 0.0f;

    for (size_t t = 0; t < numSamples; ++t)
    {
        float impulse = (t == 0) ? 1.0f : 0.0f; 
        ArraySignal x = AudioFX::ReverbUtils::split<NUM_CHANNELS>(impulse);

        float output = reverb.process(impulse);

        if (output > 0.0f)
        {
            double time = t / 44100.0;
            
            if (output > max_value)
            {
                max_value = output;
            }
            outfile << time << "," << output << "\n";
        }
    }
    
    outfile.close();

    CHECK(max_value <= 1.0f);
    std::system("python3 tests/amp_envelope.py");
}




