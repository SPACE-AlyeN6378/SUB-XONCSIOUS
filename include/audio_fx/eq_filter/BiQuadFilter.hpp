#ifndef BIQUAD_FILTER_HPP
#define BIQUAD_FILTER_HPP
#include "audio_fx/eq_filter/FilterTypeEnum.hpp"

/**
 * @brief Modes for band-pass filter behavior.
 *
 * When designing a band-pass filter two common normalization choices exist:
 * - ConstantPeakGain: normalize the filter so the peak gain at the center
 *   frequency remains constant when changing the bandwidth/Q.
 * - ConstantSkirtGain: normalize the filter so the gain far from the
 *   center frequency (the "skirts") remains constant when changing the
 *   bandwidth/Q.
 */
enum class BandPassMode
{
    /**
     * Keep the peak gain at the center frequency constant as Q/bandwidth
     * changes. Useful when the resonant peak should remain fixed.
     */
    ConstantPeakGain,

    /**
     * Keep the skirt (out-of-band) gain constant as Q/bandwidth changes.
     * Useful when the overall passband energy should remain consistent.
     */
    ConstantSkirtGain
};

struct BQCoefficients {
    double b0, b1, b2, a1, a2;
};

/**
 * @brief Simple biquad filter for computing filter coefficients and
 *        maintaining internal state for processing audio samples.
 *
 * This class holds the biquad coefficients (b0, b1, b2, a1, a2) and the
 * previous input/output samples (x1, x2, y1, y2). It provides methods to
 * configure the filter for common filter types (low-pass, high-pass,
 * band-pass, peaking, low-shelf, high-shelf) and a public setter to select
 * the active filter type and parameters.
 */
class BiQuadFilter
{
public:
    /**
     * @brief Configure coefficients for a low-pass filter.
     * @param frequency Cutoff frequency in Hz.
     * @param Q Quality factor.
     */
    void setLowPass(double frequency, double Q);

    /**
     * @brief Configure coefficients for a high-pass filter.
     * @param frequency Cutoff frequency in Hz.
     * @param Q Quality factor.
     */
    void setHighPass(double frequency, double Q);

    /**
     * @brief Configure coefficients for a band-pass filter.
     * @param frequency Center frequency in Hz.
     * @param Q Quality factor.
     */
    void setBandPass(double frequency, double Q, BandPassMode mode);

    /**
     * @brief Configure coefficients for a notch filter.
     * @param frequency Center frequency in Hz.
     * @param Q Quality factor.
     */
    void setNotch(double frequency, double Q);

    /**
     * @brief Configure coefficients for a peaking EQ filter.
     * @param frequency Center frequency in Hz.
     * @param Q Quality factor.
     * @param gainDB Gain in decibels.
     */
    void setPeaking(double frequency, double Q, double gainDB);

    /**
     * @brief Configure coefficients for a low-shelf filter.
     * @param frequency Shelf frequency in Hz.
     * @param S Shelf slope.
     * @param gainDB Gain in decibels.
     */
    void setLowShelf(double frequency, double S, double gainDB);

    /**
     * @brief Configure coefficients for a high-shelf filter.
     * @param frequency Shelf frequency in Hz.
     * @param S Shelf slope.
     * @param gainDB Gain in decibels.
     */
    void setHighShelf(double frequency, double S, double gainDB);

    /**
     * @brief Construct a new BiQuadFilter.
     * @param sampleRate Sampling rate in Hz used for coefficient calculations.
     */
    BiQuadFilter(double sampleRate);

    // Coefficients Getter
    BQCoefficients getCoefficients();

    float process(float x);
    void reset();

private:
    /* Coefficients (b0 + b1*z^-1 + b2*z^-2) / (1 + a1*z^-1 + a2*z^-2) */
    double b0, b1, b2;
    double a1, a2;

    /* Delay states for direct form I/II implementation */
    float x1 = 0.0f;
    float x2 = 0.0f;
    float y1 = 0.0f;
    float y2 = 0.0f;

    /* Sample rate for coefficient calculations. */
    double sampleRate = 44100.0;
};


#endif  // BIQUAD_FILTER_HPP