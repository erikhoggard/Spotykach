#pragma once

#include "nocopy.h"
#include "daisysp.h"

namespace spotykach {

class Reverb {
public:
    Reverb();
    ~Reverb() = default;

    void init(const float sample_rate, daisysp::ReverbSc* rsc);

    // intensity -> ReverbSc feedback (decay), mapped 0.55..0.99
    void  set_intensity(const float norm);
    float intensity() const { return _intensity; }

    // mix -> wet level in linear gain, dB-mapped -40..0 dB
    void  set_mix(const float norm);
    float mix() const { return _mix_norm; }
    float mix_lin() const { return _mix_lin; }

    // fb -> ReverbSc LP cutoff (damping), inverted so high knob = darker
    void  set_fb(const float norm);
    float fb() const { return _fb; }

    // Produces wet-only stereo. Caller scales by mix_lin() and adds to dry.
    void process(float& inout0, float& inout1);

private:
    NOCOPY(Reverb)

    daisysp::ReverbSc* _rsc;

    float _intensity;
    float _mix_norm;
    float _mix_lin;
    float _fb;
};

};
