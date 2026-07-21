#include "StateVariableFilter.hpp"
#include <cmath>
#include <numbers>

StateVariableFilter::StateVariableFilter()
    : R(1.0f), g(0.0f), state1(0.0f), state2(0.0f),
    hp(0.0f), lp(0.0f), bp(0.0f),
    sampleRate(44100.0f)
{
}

StateVariableFilter::StateVariableFilter(float cutoff, float Q, float fs)
    : state1(0.0f), state2(0.0f),
    hp(0.0f), lp(0.0f), bp(0.0f),
    sampleRate(fs)
{
    setResonance(Q);
    setCutoff(cutoff);
}

void StateVariableFilter::setCutoff(float cutoff)
{
    g = std::tan(std::numbers::pi_v<float> * cutoff / sampleRate); 
}

void StateVariableFilter::setResonance(float Q)
{
    R = 1.0f / (2.0f * Q);
}

void StateVariableFilter::setOutputMode(FilterType mode)
{
    outputMode = mode;
}

void StateVariableFilter::process(float input)
{
    // Reference: https://synthengineer.com/blog/zero-delay-feedback-filters
    //          : https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.2.pdf
    // Compute the algebraic loop
    hp = (input - (2.0f*R + g) * state1 - state2)
        / (1.0f + 2.0f*R*g + g*g);

    // Compute outputs from integrators
    float v1 = g * hp;
    bp = v1 + state1;
    float v2 = g * bp;
    lp = v2 + state2;

    // Update states (equiv: s = 2*out - s)
    state1 = bp + v1;
    state2 = lp + v2;
}

void StateVariableFilter::reset()
{
    state1 = 0.0f;
    state2 = 0.0f;
    hp = 0.0f;
    bp = 0.0f;
    lp = 0.0f;
}

float StateVariableFilter::getHighPass() {
    return hp; 
}
float StateVariableFilter::getBandPass() { return bp; }
float StateVariableFilter::getLowPass()  { return lp; }
