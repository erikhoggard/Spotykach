#include "fx.reverb.h"
#include "../common.h"

using namespace spotykach;
using namespace infrasonic;
using namespace daisysp;

Reverb::Reverb():
_rsc       { nullptr },
_intensity { .35f },
_mix_norm  { .5f },
_mix_lin   { 0.f },
_fb        { .5f }
{}

void Reverb::init(const float sample_rate, daisysp::ReverbSc* rsc)
{
    _rsc = rsc;
    _rsc->Init(sample_rate);
    set_intensity(_intensity);
    set_mix(_mix_norm);
    set_fb(_fb);
}

void Reverb::set_intensity(const float norm)
{
    _intensity = fclamp(norm, 0.f, 1.f);
    if (_rsc) _rsc->SetFeedback(fmap(_intensity, 0.80f, 0.99f));
}

void Reverb::set_mix(const float norm)
{
    _mix_norm = fclamp(norm, 0.f, 1.f);
    _mix_lin = dbfs2lin(fmap(_mix_norm, -20.f, 0.f));
}

void Reverb::set_fb(const float norm)
{
    _fb = fclamp(norm, 0.f, 1.f);
    // High knob = darker. Invert so high _fb maps to lower cutoff.
    if (_rsc) _rsc->SetLpFreq(fmap(1.f - _fb, 800.f, 8000.f));
}

void Reverb::process(float& inout0, float& inout1)
{
    if (!_rsc) return;
    float outL = 0.f, outR = 0.f;
    _rsc->Process(inout0, inout1, &outL, &outR);
    inout0 = outL;
    inout1 = outR;
}
