#pragma once

#include <raylib.h>
#include <array>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace F1Game {

enum class SurfaceType {
    ASPHALT = 0,
    GRASS,
    RUMBLE,
    COUNT
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void Init();
    void Update(float rpm, float speed_kmh, float wetness, const float* slip, const SurfaceType* surface,
                bool isPitLane, bool drsAvailable, bool gearChange);
    void PlaySFX(const char* path);  // For radio/UI one-shots

private:
    void UpdateEngine(float rpm);
    void UpdateTires(float speed_kmh, const float* slip, const SurfaceType* surface);
    void UpdateWeather(float wetness);
    void UpdateAmbience();
    void CheckTriggers(bool isPitLane, bool drsAvailable, bool gearChange);

    ma_engine m_engine;

    // Engine layers (looping)
    ma_sound m_engineBase;
    ma_sound m_turboWhistle;
    ma_sound m_intake;

    // Tires (per wheel, looping)
    std::array<ma_sound, 4> m_squealDry;
    std::array<ma_sound, 4> m_squealWet;
    std::array<ma_sound, 4> m_rumble;
    std::array<ma_sound, 4> m_grass;

    // Weather (looping)
    ma_sound m_rainLight;
    ma_sound m_rainHeavy;
    std::array<ma_sound, 4> m_splash;

    // Ambience (looping)
    ma_sound m_crowd;

    // Radio/UI/SFX (one-shot)
    ma_sound m_radioPitConfirm;
    ma_sound m_radioUndercut;
    ma_sound m_radioDRS;
    ma_sound m_uiPitClick;
    ma_sound m_gearShift;
};

}


