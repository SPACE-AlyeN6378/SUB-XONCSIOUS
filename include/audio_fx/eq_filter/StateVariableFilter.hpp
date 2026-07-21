#ifndef STATE_VARIABLE_FILTER_HPP
#define STATE_VARIABLE_FILTER_HPP
#include "audio_fx/eq_filter/FilterTypeEnum.hpp"

class StateVariableFilter {
public:
    StateVariableFilter();
    StateVariableFilter(float cutoff, float Q, float fs);

    // Setters
    void setCutoff(float cutoff);
    void setResonance(float Q);
    void setOutputMode(FilterType mode);

    
    /**
     * @brief Processes one audio sample through the zero-delay feedback TPT state variable filter.
     *
     * The filter uses a trapezoidal (TPT) integrator formulation with zero-delay feedback
     * to calculate the high-pass, band-pass, and low-pass responses. Unlike conventional
     * Chamberlin state variable filters, the feedback loop is solved algebraically, avoiding
     * an additional unit delay in the feedback path.
     *
     * The implementation follows the zero-delay feedback state variable filter approach
     * described by Synth Engineer:
     * https://synthengineer.com/blog/zero-delay-feedback-filters
     *
     * @param input The input audio sample.
     *
     * @note The filter states are updated internally each call. The generated low-pass,
     * band-pass, and high-pass outputs can be retrieved using their respective getter
     * functions after processing.
     *
     * @see getLowPass()
     * @see getBandPass()
     * @see getHighPass()
     *
     * @return void
     */
    void process(float input);

    void reset();

    // Getters
    float getHighPass();
    float getLowPass();
    float getBandPass();


private:    
    float R = 1.0f;           // Damping/resonance factor
    float g = 0.0f;           // TPT Integrator coefficient g = tan(\omega_c T / 2)

    // TPT States
    float state1 = 0.0f;
    float state2 = 0.0f;

    // Output values
    float hp = 0.0f;
    float bp = 0.0f;
    float lp = 0.0f;

    float sampleRate;
    FilterType outputMode = FilterType::LowPass;
};

#endif  // STATE_VARIABLE_FILTER_HPP

