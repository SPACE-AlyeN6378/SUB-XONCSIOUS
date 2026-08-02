#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include "audio_fx/delay/Delay.hpp"
#include <cmath>

class Compressor
{
public:
    Compressor();
    Compressor(float threshold_, float ratio_, float attackTimeMs, float releaseTimeMs, float rmsTimeMs, double sampleRate_);

    float process(float input);
    void reset();

    // ****** SETTERS ******
    // Set the sample rate and update coefficients accordingly
    void setSampleRate(double value);

    void setAttackTime(double attackTimeMs);
    void setReleaseTime(double releaseTimeMs);
    void setRMSTime(double rmsTimeMs);

    void setAttackCoeff(float coeff) { attackCoeff = coeff; }
    void setReleaseCoeff(float coeff) { releaseCoeff = coeff; }
    void setRMSCoeff(float coeff) { rmsCoeff = coeff; }
    
    // ****** GETTERS ******
    float getRMS(bool inDb = false) const;
    float getGain(bool inDb = false) const;

private:
    float rms = 0.0f;
    float rmsCoeff;
    float gain = 1.0f;

    float threshold;
    float ratio;

    float attackCoeff;
    float releaseCoeff;

    double sampleRate;

    Delay<1024> lookahead;

    // Converts time in milliseconds to a coefficient for the attack/release filter and RMS detector
    static float timeToCoeff(float timeMs, double sampleRate)
    {
        return 1.0f - exp(-2.2f / (timeMs * 0.001f * static_cast<float>(sampleRate)));
    }

    static float coeffToTime(float coeff, double sampleRate)
    {
        return -2200.0f / (static_cast<float>(sampleRate) * log(1.0f - coeff));
    }
};


#endif