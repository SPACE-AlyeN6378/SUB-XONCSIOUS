#include "BiQuadFilter.hpp"
#include <cmath>

void BiQuadFilter::setLowPass(double frequency, double Q)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2.0 * Q);

    double a0 = 1 + alpha;
    a1 = -2 * cos(omega) / a0;
    a2 = (1 - alpha) / a0;

    b0 = (1 - cos(omega)) / (2.0 * a0);
    b1 = (1 - cos(omega)) / a0;
    b2 = (1 - cos(omega)) / (2.0 * a0);
}


void BiQuadFilter::setHighPass(double frequency, double Q)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2.0 * Q);

    double a0 = 1 + alpha;
    a1 = -2 * cos(omega) / a0;
    a2 = (1 - alpha) / a0;

    b0 = (1 + cos(omega)) / (2.0 * a0);
    b1 = -(1 + cos(omega)) / a0;
    b2 = (1 + cos(omega)) / (2.0 * a0);
}


void BiQuadFilter::setBandPass(double frequency, double Q, BandPassMode mode)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2.0 * Q);

    double a0 = 1 + alpha;
    a1 = (-2 * cos(omega)) / a0;
    a2 = (1 - alpha) / a0;

    switch (mode) {
        case BandPassMode::ConstantPeakGain:
            b0 = alpha / a0;
            b1 = 0.0;
            b2 = -alpha / a0;
            break;

        case BandPassMode::ConstantSkirtGain:
            b0 = Q * alpha / a0;
            b1 = 0.0;
            b2 = -Q * alpha / a0;
            break;
    }
}


void BiQuadFilter::setNotch(double frequency, double Q)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2.0 * Q);

    double a0 = 1 + alpha;
    a1 = -2*cos(omega) / a0;
    a2 = (1 - alpha) / a0;

    b0 = 1.0 / a0;
    b1 = -2*cos(omega) / a0;
    b2 = 1.0 / a0;
}


void BiQuadFilter::setPeaking(double frequency, double Q, double gainDB)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double alpha = sin(omega) / (2.0 * Q);
    double     A = pow(10.0, gainDB / 40);

    double a0 = 1 + alpha / A;
    a1 = -2 * cos(omega) / a0;
    a2 = (1 - alpha / A) / a0;

    b0 = (1 + alpha * A) / a0;
    b1 = (-2 * cos(omega)) / a0;
    b2 = (1 - alpha * A) / a0;
}

//<*lowshelf>
void BiQuadFilter::setLowShelf(double frequency, double S, double gainDB)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double     A = pow(10.0, gainDB / 40);
    double alpha = sin(omega) / 2 * sqrt((A + 1/A) * (1/S - 1) + 2);

    double a0 = (A+1) + (A-1)*cos(omega) + 2*sqrt(A)*alpha;
    a1 = -2*( (A-1) + (A+1)*cos(omega) );                   a1 /= a0;
    a2 = (A+1) + (A-1)*cos(omega) - 2*sqrt(A)*alpha;        a2 /= a0;

    b0 = A*( (A+1) - (A-1)*cos(omega) + 2*sqrt(A)*alpha );  b0 /= a0;
    b1 = 2*A*( (A-1) - (A+1)*cos(omega) );                  b1 /= a0;
    b2 = A*( (A+1) - (A-1)*cos(omega) - 2*sqrt(A)*alpha );  b2 /= a0;
}
//</lowshelf>

/*<*highshelf>*/
void BiQuadFilter::setHighShelf(double frequency, double S, double gainDB)
{
    double omega = 2.0 * M_PI * frequency / sampleRate;
    double     A = pow(10.0, gainDB / 40);
    double alpha = sin(omega) / 2 * sqrt((A + 1/A) * (1/S - 1) + 2);

    double a0 = (A+1) - (A-1)*cos(omega) + 2*sqrt(A)*alpha;
    a1 = 2 * ((A-1) - (A+1)*cos(omega));                   a1 /= a0;
    a2 = (A+1) - (A-1)*cos(omega) - 2*sqrt(A)*alpha;        a2 /= a0;

    b0 = A*( (A+1) + (A-1)*cos(omega) + 2*sqrt(A)*alpha );  b0 /= a0;
    b1 = -2*A*( (A-1) + (A+1)*cos(omega) );                 b1 /= a0;
    b2 = A*( (A+1) + (A-1)*cos(omega) - 2*sqrt(A)*alpha );  b2 /= a0;
}
/*</highshelf>*/

// Constructor
BiQuadFilter::BiQuadFilter(double sampleRate)
    : sampleRate(sampleRate),
    b0(1.0), b1(0.0), b2(0.0),
    a1(0.0), a2(0.0)
{
}


BQCoefficients BiQuadFilter::getCoefficients()
{
    BQCoefficients coeffs;
    coeffs.b0 = b0;
    coeffs.b1 = b1;
    coeffs.b2 = b2;
    coeffs.a1 = a1;
    coeffs.a2 = a2;
    return coeffs;
}

float BiQuadFilter::process(float x)
{
    float y = static_cast<float>(b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2);
    
    // Update the states
    y2 = y1; x2 = x1;
    y1 = y; x1 = x;

    return y;
}

void BiQuadFilter::reset()
{
    x1 = 0.0f;
    x2 = 0.0f;
    y1 = 0.0f;
    y2 = 0.0f;
}
