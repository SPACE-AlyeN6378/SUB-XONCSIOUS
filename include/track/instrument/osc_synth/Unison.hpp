#ifndef UNISON_HPP
#define UNISON_HPP

#include "audio_source/oscillator/Oscillator.hpp"
#include <vector>
#include <stdexcept>
#include <random>

namespace SubXoncsious {
namespace SynthUtils
{

struct UnisonVoice
{
    Oscillator oscillator;

    double baseFrequency = 440.0;
    double detuneCents = 0.0;
    float gain = 1.0f;
    float masterGain = 0.7f;
    float pan = 0.0f;          // Future
    double phase = 0.0f;

    void setBaseFrequency(double frequency)
    {
        baseFrequency = frequency;
        updateFrequency();
    }

    void setDetune(double cents)
    {
        detuneCents = cents;
        updateFrequency();
    }

    void setGain(float gain_)
    {
        gain = gain_;
        oscillator.setAmplitude(gain_ * masterGain);
        oscillator.updateCache();
    }

    void setMasterGain(float masterGain_)
    {
        masterGain = masterGain_;
        oscillator.setAmplitude(gain * masterGain_);
        oscillator.updateCache();
    }

    void setShape(OscShape shape_)
    {
        oscillator.setShape(shape_);
    }

    void setSampleRate(double sampleRate)
    {
        oscillator.setSampleRate(sampleRate);
    }

    void setPhase(double phase)
    {
        this->phase = phase;
        oscillator.setPhase(phase);
    }

    float generate()
    {
        return oscillator.generate();
    }



private:
    void updateFrequency()
    {
        const double ratio = std::pow(2.0, detuneCents / 1200.0);
        oscillator.setFrequency(baseFrequency * ratio);
        oscillator.updateCache();
    }
};

enum class BlendLaw
{
    Linear,
    EqualPower,
    Parabolic
};

class Unison
{
public:
    // Default constructor
    Unison()
    {
        setVoices(1);
        setFrequency(440.0);
        setDetune(0.0);
        setShape(OscShape::SAWTOOTH);
        setGain(0.7f);
        setSampleRate(44100.0);
    }

    Unison(int voices, double frequency, OscShape shape,
        double detune = 0.0, float gain = 0.7f, double sampleRate = 44100.0)
    {
        setVoices(voices);
        setFrequency(frequency);
        setDetune(detune);
        setShape(shape);
        setGain(gain);
        setSampleRate(sampleRate);
    }

    void setFrequency(double frequency_)
    {
        frequency = frequency_;
        for (UnisonVoice& voice: voices)
        {
            voice.setBaseFrequency(frequency);
        }
    }

    void setShape(OscShape shape_)
    {
        shape = shape_;
        for (UnisonVoice& voice: voices)
        {
            voice.setShape(shape_);
        }
    }

    void setVoices(int num)
    {
        if (num < 1 || num > 7)
            throw std::invalid_argument(
                "Number of voices must be between 1 and 7"
            );

        // voices.clear();
        // voices.reserve(static_cast<std::size_t>(num));

        // for (int i = 0; i < num; ++i)
        // {
        //     voices.emplace_back();
        // }
        voices.resize(num);
        numVoices = num;

        updateDetunes();
        setFrequency(frequency);
        setGain(gain);
        setSampleRate(sampleRate);
        setShape(shape);
        randomizePhases();
    }

    void setDetune(double amount) noexcept
    { 
        detuneAmount = amount;
        updateDetunes();
    }

    void setBlend(float blend)
    {
        if (numVoices == 1)
        {
            voices[0].setGain(1.0f);
            return;
        }

        float centreAmp;
        float sideAmp;

        if (blendLaw == BlendLaw::EqualPower)
        {
            if (numVoices % 2 == 0)
            {
                centreAmp = std::sqrt((1.0f - blend) / 2.0f);
                sideAmp = std::sqrt(blend / (numVoices - 2));
            }
            else
            {
                centreAmp = std::sqrt(1.0f - blend);
                sideAmp = std::sqrt(blend / (numVoices - 1));
            }
        }
        else if (blendLaw == BlendLaw::Linear)
        {
            if (numVoices % 2 == 0)
            {
                centreAmp = (1.0f - blend) / 2.0f;
                sideAmp = blend / (numVoices - 2);
            }
            else
            {
                centreAmp = 1.0f - blend;
                sideAmp = blend / (numVoices - 1);
            }
        }
        else
        {
            throw std::invalid_argument(
                "Currently unsupported at the moment"
            );
        }
        
        if (numVoices % 2 == 1)
        {
            int centre = numVoices / 2;

            for (int i = 0; i < numVoices; ++i)
            {
                voices[i].setGain(i == centre ? centreAmp : sideAmp);
            }
        }
        else
        {
            int centre1 = numVoices / 2 - 1;
            int centre2 = numVoices / 2;

            for (int i = 0; i < numVoices; ++i)
            {
                bool isCentre = (i == centre1 || i == centre2);

                voices[i].setGain(isCentre ? centreAmp : sideAmp);
            }
        }
    }

    void setGain(float value)
    {
        gain = value;
        for (UnisonVoice& voice: voices)
            voice.setMasterGain(value);
    }

    void setSampleRate(double value)
    {
        sampleRate = value;
        for (UnisonVoice& voice: voices)
            voice.setSampleRate(value);
    }

    void randomizePhases()
    {
        for (UnisonVoice& voice: voices)
            voice.setPhase(randomPhase());
    }

    float generate()
    {
        float sample = 0.0f;
        for (UnisonVoice& voice: voices)
            sample += voice.generate();

        return sample;
    }

private:
    std::vector<UnisonVoice> voices;
    int numVoices = 0;
    double detuneAmount = 0.0;
    double frequency = 440.0;
    float gain = 0.7f;
    double sampleRate = 44100.0;
    OscShape shape = OscShape::SAWTOOTH;

    std::mt19937 rng{std::random_device{}()};

    BlendLaw blendLaw = BlendLaw::EqualPower;

    void updateDetunes()
    {
        double centre = (numVoices - 1) / 2.0;

        for (int i = 0; i < numVoices; ++i)
        {
            voices[i].setDetune(
                (i - centre) * detuneAmount
            );
        }
    }

    double randomPhase()
    {
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        return dist(rng);
    }
};



}   // namespace SynthUtils
}   // namespace SubXoncsious



#endif  // UNISON_HPP