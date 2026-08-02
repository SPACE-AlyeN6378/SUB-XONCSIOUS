#include "Compressor.hpp"


constexpr size_t LOOKAHEAD_SAMPLES = 150; // 150 samples lookahead for the compressor 

Compressor::Compressor()
    : threshold(0.5f), ratio(4.0f), attackCoeff(2.0), releaseCoeff(100.0), rmsCoeff(10.0),
    lookahead(LOOKAHEAD_SAMPLES), sampleRate(44100.0)
{
    setSampleRate(44100.0);
}


Compressor::Compressor(float threshold_, float ratio_, float attackTimeMs, float releaseTimeMs, float rmsTimeMs, double sampleRate_)
    : threshold(threshold_), ratio(ratio_), 
    attackCoeff(timeToCoeff(attackTimeMs, sampleRate_)), releaseCoeff(timeToCoeff(releaseTimeMs, sampleRate_)),
    lookahead(LOOKAHEAD_SAMPLES), rmsCoeff(timeToCoeff(rmsTimeMs, sampleRate_))
{
    setSampleRate(sampleRate_);
}

// <*process_compressor>
float Compressor::process(float input)
{
    // RMS detector
    rms = (1 - rmsCoeff) * rms 
          + rmsCoeff * input * input;

    float level = 10 * log10(rms + 1e-12);

    // Compressor curve
    float gainDB = 0;

    if (level > threshold)
    {
        float over = level - threshold;
        gainDB = -(over - over / ratio);
    }

    float targetGain = pow(10, gainDB / 20.0f);

    // Attack/release
    float coeff = (targetGain < gain) ? attackCoeff : releaseCoeff;
    gain = (1-coeff)*gain + coeff*targetGain;

    // Lookahead output
    return gain * lookahead.process(input);
}
// </process_compressor>

void Compressor::reset()
{
    rms = 0.0f;
    gain = 1.0f;
    lookahead.reset();
}


void Compressor::setSampleRate(double value)
{
    float attackTimeMs = coeffToTime(attackCoeff, value);
    float releaseTimeMs = coeffToTime(releaseCoeff, value);
    float rmsTimeMs = coeffToTime(rmsCoeff, value);

    sampleRate = value;
    
    setAttackTime(attackTimeMs);
    setReleaseTime(releaseTimeMs);
    setRMSTime(rmsTimeMs);
}


void Compressor::setAttackTime(double attackTimeMs)
{
    attackCoeff = timeToCoeff(attackTimeMs, sampleRate);
}


void Compressor::setReleaseTime(double releaseTimeMs)
{
    releaseCoeff = timeToCoeff(releaseTimeMs, sampleRate);
}


void Compressor::setRMSTime(double rmsTimeMs)
{
    rmsCoeff = timeToCoeff(rmsTimeMs, sampleRate);
}

float Compressor::getRMS(bool inDb) const
{
    if (!inDb)
        return rms;

    return 10.0f * log10(rms + 1e-12f);
}

float Compressor::getGain(bool inDb) const
{
    if (!inDb)
        return gain;

    return 20.0f * log10(gain + 1e-12f);
}

