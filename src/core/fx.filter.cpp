#include "fx.filter.h"
#include "../common.h"
#include "daisysp.h"

using namespace spotykach;
using namespace infrasonic;
using namespace daisysp;

Filter::Filter():
_sample_rate    { 0.f },
_cutoff_norm    { 0.5f },
_reso_norm      { 0.3f },
_q              { 1.f },
_target_wet_kof { 0.f },
_mode           { Mode::Bypass }
{}

void Filter::init(const float sample_rate)
{
    _sample_rate = sample_rate;

    _lpf.Init(sample_rate);
    _hpf.Init(sample_rate);

    _wet_smooth.init(sample_rate, kWetSmoothS);
    _wet_smooth.reset(0.f);

    // Initial Q from default _reso_norm
    _q = fmap(_reso_norm, kQMin, kQMax, Mapping::EXP);

    _apply();
}

void Filter::set_cutoff(const float norm)
{
    _cutoff_norm = fclamp(norm, 0.f, 1.f);
    _apply();
}

void Filter::set_resonance(const float norm)
{
    _reso_norm = fclamp(norm, 0.f, 1.f);
    _q = fmap(_reso_norm, kQMin, kQMax, Mapping::EXP);
    _apply();
}

void Filter::_apply()
{
    if (_cutoff_norm < kDeadzoneLo) {
        _mode = Mode::LowPass;
        const float t = _cutoff_norm / kDeadzoneLo;          // 0..1 across LPF side
        const float hz = fmap(t, kFreqMin, kFreqMax, Mapping::LOG);
        _lpf.SetParams(hz, _q);
        _target_wet_kof = 1.f;
    }
    else if (_cutoff_norm > kDeadzoneHi) {
        _mode = Mode::HighPass;
        const float t = (_cutoff_norm - kDeadzoneHi) / (1.f - kDeadzoneHi);
        const float hz = fmap(t, kFreqMin, kFreqMax, Mapping::LOG);
        _hpf.SetParams(hz, _q);
        _target_wet_kof = 1.f;
    }
    else {
        _mode = Mode::Bypass;
        _target_wet_kof = 0.f;
    }
}

// Note on mode-switch hygiene:
//   When crossing LPF<->HPF (via the dead-zone), the previously-active cascade
//   retains its internal state (s1_/s2_ in BiquadSection). The wet/dry smoother
//   masks the re-engagement: target_wet_kof goes to 0 in the dead-zone and
//   ramps back to 1 on the other side, hiding any transient. BiquadCascade::Init()
//   in this codebase resets coefficients but does NOT clear section state, so
//   calling it here would be misleading. The smoother is the actual click guard.

void Filter::process(float& inout0, float& inout1)
{
    const float wet_kof = _wet_smooth.process(_target_wet_kof);

    // Cheap fully-dry exit
    if (wet_kof < 1e-4f && _target_wet_kof == 0.f) return;

    float wet0 = inout0;
    float wet1 = inout1;
    switch (_mode) {
        case Mode::LowPass:  _lpf.ProcessStereo(wet0, wet1); break;
        case Mode::HighPass: _hpf.ProcessStereo(wet0, wet1); break;
        case Mode::Bypass:   break;
    }
    inout0 = wet0 * wet_kof + inout0 * (1.f - wet_kof);
    inout1 = wet1 * wet_kof + inout1 * (1.f - wet_kof);
}
