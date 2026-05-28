#pragma once

#include "../nocopy.h"
#include "fx.drive.h"
#include "fx.reduce.h"
#include "fx.reverb.h"
#include "fx.filter.h"
#include "biquad.h"
#include "echo.h"
#include "softswitch.h"

namespace spotykach {

class Fx {
public:
    enum class GritMode: uint8_t {
        Drive,
        Reduce
    };

    enum class FluxMode: uint8_t {
        Echo,
        Reverb
    };

    struct Params {
        float sample_rate;
        float** delay_buf;
        daisysp::ReverbSc* reverb_instance;
    };

    static constexpr size_t kFeedbackDelayBufferLength  { 10000 }; 
    static constexpr uint8_t kDelayMaxSeconds           { 5 };
    static constexpr size_t kEchoDelayBufferLength      { 48000 * kDelayMaxSeconds };

    Fx();
    ~Fx() = default;

    void init(const Params);
    void process(float& inout0, float& inout1);

    // GRIT ///////////////////////////////////////////
    GritMode grit_mode() const { return _grit_mode; }
    void switch_grit_mode();
    
    bool is_grit_on() const { return _grit_switch.is_on(); }
    void set_grit_on(const bool);
    void toggle_grit_lock();

    float grit_intensity();
    void set_grit_intensity(const float norm);
    
    float grit_mix();
    void set_grit_mix(const float norm);
    
    // FLUX ///////////////////////////////////////////
    bool is_flux_on() const { return _flux_switch.is_on(); }
    void set_flux_on(const bool);
    void toggle_flux_lock();

    FluxMode flux_mode() const { return _flux_mode; }
    void switch_flux_mode();
    void set_flux_mode(const FluxMode mode);

    float flux_intensity() const;
    void set_flux_intensity(const float norm);

    float flux_mix() const;
    void set_flux_mix(const float norm);

    float flux_fb() const;
    void set_flux_fb(const float norm);

    // FILTER /////////////////////////////////////////
    void  set_filter_cutoff(const float norm);
    float filter_cutoff() const;

    void  set_filter_resonance(const float norm);
    float filter_resonance() const;

private:
    NOCOPY(Fx)

    void _apply_flux_fb();
    void _apply_flux_mix();
    void _apply_flux_int(const bool hard = false);

    Drive _drive;
    Reduce _reduce;
    Reverb _reverb;
    Filter _filter;
    infrasonic::EchoDelay<kEchoDelayBufferLength> _echo_delay[2];
    SoftSwitch _flux_switch;
    SoftSwitch _grit_switch;
    
    float _flux_int;
    float _flux_mix;
    float _flux_mix_norm;
    float _flux_fb;
    float _reverb_int;
    float _reverb_mix_norm;
    float _reverb_fb;
    FluxMode _flux_mode;
    bool _flux_on;
    bool _flux_lock;

    GritMode _grit_mode;
    bool _grit_on;
    bool _grit_lock;
    
};

};