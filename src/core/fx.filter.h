#pragma once

#include "nocopy.h"
#include "biquad.h"
#include "smooth.h"
#include <cstdint>

namespace spotykach {

class Filter {
public:
    Filter();
    ~Filter() = default;

    void init(const float sample_rate);

    // norm 0..1, 0.5 = bypass center.
    // Left of dead-zone -> LPF, right of dead-zone -> HPF.
    void  set_cutoff(const float norm);
    float cutoff() const { return _cutoff_norm; }

    // norm 0..1 -> Q ~0.5..8.0 (quadratic taper).
    void  set_resonance(const float norm);
    float resonance() const { return _reso_norm; }

    // In-place stereo process. Always-on. Bypass is implicit via dead-zone.
    void process(float& inout0, float& inout1);

private:
    NOCOPY(Filter)

    enum class Mode : uint8_t { Bypass, LowPass, HighPass };

    void _apply();

    static constexpr float kDeadzoneLo  = 0.47f;
    static constexpr float kDeadzoneHi  = 0.53f;
    static constexpr float kFreqMin     = 30.f;
    static constexpr float kFreqMax     = 18000.f;
    static constexpr float kQMin        = 0.5f;
    static constexpr float kQMax        = 8.0f;
    static constexpr float kWetSmoothS  = 0.010f;

    infrasonic::LPF24 _lpf;
    infrasonic::HPF24 _hpf;
    OnePoleSmoother   _wet_smooth;

    float _sample_rate;
    float _cutoff_norm;
    float _reso_norm;
    float _q;
    float _target_wet_kof;
    Mode  _mode;
};

}
