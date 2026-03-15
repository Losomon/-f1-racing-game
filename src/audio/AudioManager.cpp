#include "AudioManager.hpp"

namespace F1Game {

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    Shutdown();
}

void AudioManager::Init() {
    if (ma_engine_init(NULL, &m_engine) != MA_SUCCESS) {
        printf("Failed to init miniaudio engine\n");
        return;
    }

    const char* basePath = "assets/audio/textaudio/";

    // Engine sounds (looping)
    if (ma_sound_init_from_file(&m_engine, (std::string(basePath) + "engine/v6_hybrid_base.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_engineBase) == MA_SUCCESS) {
        ma_sound_set_looping(&m_engineBase, MA_TRUE);
        ma_sound_set_volume(&m_engineBase, 0.7f);
        ma_sound_start(&m_engineBase);
    }
    if (ma_sound_init_from_file(&m_engine, (std::string(basePath) + "engine/turbo_whistle.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_turboWhistle) == MA_SUCCESS) {
        ma_sound_set_looping(&m_turboWhistle, MA_TRUE);
        ma_sound_set_volume(&m_turboWhistle, 0.0f);  // Start off
    }
    if (ma_sound_init_from_file(&m_engine, (std::string(basePath) + "engine/intake.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_intake) == MA_SUCCESS) {
        ma_sound_set_looping(&m_intake, MA_TRUE);
        ma_sound_set_volume(&m_intake, 0.3f);
        ma_sound_start(&m_intake);
    }

    // Tires (per wheel, looping)
    for (int i = 0; i < 4; ++i) {
        std::string dryPath = std::string(basePath) + "tires/squeal_dry.wav";
        std::string wetPath = std::string(basePath) + "tires/squeal_wet.wav";
        std::string rumblePath = std::string(basePath) + "tires/rumble.wav";
        std::string grassPath = std::string(basePath) + "tires/grass.wav";

        ma_sound_init_from_file(&m_engine, dryPath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_squealDry[i]);
        ma_sound_set_looping(&m_squealDry[i], MA_TRUE);
        ma_sound_set_volume(&m_squealDry[i], 0.0f);

        ma_sound_init_from_file(&m_engine, wetPath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_squealWet[i]);
        ma_sound_set_looping(&m_squealWet[i], MA_TRUE);
        ma_sound_set_volume(&m_squealWet[i], 0.0f);

        ma_sound_init_from_file(&m_engine, rumblePath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_rumble[i]);
        ma_sound_set_looping(&m_rumble[i], MA_TRUE);
        ma_sound_set_volume(&m_rumble[i], 0.0f);

        ma_sound_init_from_file(&m_engine, grassPath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_grass[i]);
        ma_sound_set_looping(&m_grass[i], MA_TRUE);
        ma_sound_set_volume(&m_grass[i], 0.0f);
    }

    // Weather
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "weather/rain_light.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_rainLight);
    ma_sound_set_looping(&m_rainLight, MA_TRUE);
    ma_sound_set_volume(&m_rainLight, 0.0f);
    
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "weather/rain_heavy.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_rainHeavy);
    ma_sound_set_looping(&m_rainHeavy, MA_TRUE);
    ma_sound_set_volume(&m_rainHeavy, 0.0f);
    
    for (int i = 0; i < 4; ++i) {
        ma_sound_init_from_file(&m_engine, (std::string(basePath) + "weather/splash.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_splash[i]);
        ma_sound_set_looping(&m_splash[i], MA_TRUE);
        ma_sound_set_volume(&m_splash[i], 0.0f);
    }

    // Ambience
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "crowd/crowd_ambience.wav").c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &m_crowd);
    ma_sound_set_looping(&m_crowd, MA_TRUE);
    ma_sound_set_volume(&m_crowd, 0.2f);
    ma_sound_start(&m_crowd);

    // One-shots (preload)
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "radio/radio_pit_confirm.wav").c_str(), 0, NULL, NULL, &m_radioPitConfirm);
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "radio/radio_undercut.wav").c_str(), 0, NULL, NULL, &m_radioUndercut);
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "radio/radio_drs_available.wav").c_str(), 0, NULL, NULL, &m_radioDRS);
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "ui/pit_stop_click.wav").c_str(), 0, NULL, NULL, &m_uiPitClick);
    ma_sound_init_from_file(&m_engine, (std::string(basePath) + "sfx/gear_shift.wav").c_str(), 0, NULL, NULL, &m_gearShift);

    printf("AudioManager initialized. Check console for missing files.\n");
}


void AudioManager::Update(float rpm, float speed_kmh, float wetness, const float* slip, const SurfaceType* surface,
                bool isPitLane, bool drsAvailable, bool gearChange) {
    UpdateEngine(rpm);
    UpdateTires(speed_kmh, slip, surface);
    UpdateWeather(wetness);
    UpdateAmbience();
    CheckTriggers(isPitLane, drsAvailable, gearChange);
}

void AudioManager::UpdateEngine(float rpm) {
    // V6 hybrid base RPM pitch/volume
    float basePitch = std::clamp(rpm / 14000.0f * 2.2f, 0.4f, 2.2f);
    ma_sound_set_pitch(&m_engineBase, basePitch);
    float baseVol = std::clamp(rpm / 15000.0f, 0.0f, 0.8f);
    ma_sound_set_volume(&m_engineBase, baseVol);

    // Turbo whistle (high RPM)
    float turboVol = std::max(0.0f, (rpm - 12000.0f) / 3000.0f * 0.6f);
    ma_sound_set_volume(&m_turboWhistle, turboVol);
    if (turboVol > 0.01f) ma_sound_start(&m_turboWhistle);
    else ma_sound_set_volume(&m_turboWhistle, 0.0f);

    // Intake always subtle
    ma_sound_set_volume(&m_intake, 0.25f);
}

void AudioManager::UpdateTires(float speed_kmh, const float* slip, const SurfaceType* surface) {
    float speed = speed_kmh * 0.27778f;  // kmh to m/s
    for (int i = 0; i < 4; ++i) {
        // Squeal base on slip angle
        float squealVol = std::max(0.0f, (std::abs(slip[i]) - 0.05f) * speed * 0.4f);
        ma_sound_set_volume(&m_squealDry[i], squealVol * (1.0f - wetness));
        ma_sound_set_volume(&m_squealWet[i], squealVol * wetness);

        // Surface specific
        float surfaceVol = speed * 0.2f;
        ma_sound_set_volume(&m_rumble[i], (surface[i] == SurfaceType::RUMBLE ? surfaceVol : 0.0f));
        ma_sound_set_volume(&m_grass[i], (surface[i] == SurfaceType::GRASS ? surfaceVol : 0.0f));
    }
}

void AudioManager::UpdateWeather(float wetness) {
    // Blend rain types
    ma_sound_set_volume(&m_rainLight, std::clamp(wetness * 0.8f, 0.0f, 0.8f));
    ma_sound_set_volume(&m_rainHeavy, std::clamp((wetness - 0.5f) * 1.2f, 0.0f, 0.6f));
}

void AudioManager::UpdateAmbience() {
    // Crowd constant for now
    ma_sound_set_volume(&m_crowd, 0.15f);
}

void AudioManager::CheckTriggers(bool isPitLane, bool drsAvailable, bool gearChange) {
    static bool lastDRS = false;
    if (drsAvailable && !lastDRS) {
        ma_sound_start(&m_radioDRS);
    }
    lastDRS = drsAvailable;

    if (isPitLane) ma_sound_start(&m_radioPitConfirm);
    if (gearChange) ma_sound_start(&m_gearShift);
}

void AudioManager::PlaySFX(const char* path) {
    ma_sound tempSound;
    if (ma_sound_init_from_file(&m_engine, path, 0, NULL, NULL, &tempSound) == MA_SUCCESS) {
        ma_sound_start(&tempSound);
    }
}


void AudioManager::Shutdown() {
    // Engine
    ma_sound_uninit(&m_engineBase);
    ma_sound_uninit(&m_turboWhistle);
    ma_sound_uninit(&m_intake);

    // Tires
    for (int i = 0; i < 4; ++i) {
        ma_sound_uninit(&m_squealDry[i]);
        ma_sound_uninit(&m_squealWet[i]);
        ma_sound_uninit(&m_rumble[i]);
        ma_sound_uninit(&m_grass[i]);
        ma_sound_uninit(&m_splash[i]);
    }

    // Weather
    ma_sound_uninit(&m_rainLight);
    ma_sound_uninit(&m_rainHeavy);

    // Ambience
    ma_sound_uninit(&m_crowd);

    // SFX
    ma_sound_uninit(&m_radioPitConfirm);
    ma_sound_uninit(&m_radioUndercut);
    ma_sound_uninit(&m_radioDRS);
    ma_sound_uninit(&m_uiPitClick);
    ma_sound_uninit(&m_gearShift);

    ma_engine_uninit(&m_engine);
    printf("AudioManager shutdown complete.\n");
}


} // namespace F1Game
