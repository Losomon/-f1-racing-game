#pragma once

namespace F1Game {
namespace Physics {

// Realistic(ish) F1 constants (tunable).
struct F1Constants {
    // Mass
    static constexpr float MASS_NO_FUEL = 740.0f;        // kg - min regs (approx)
    static constexpr float DEFAULT_FUEL_LOAD = 110.0f;   // kg

    // Dimensions
    static constexpr float WHEELBASE = 3.6f;             // m
    static constexpr float TRACK_WIDTH_FRONT = 1.8f;     // m
    static constexpr float TRACK_WIDTH_REAR = 1.7f;      // m
    static constexpr float COG_HEIGHT = 0.4f;            // m
    static constexpr float WHEEL_RADIUS = 0.33f;         // m

    // Weight distribution
    static constexpr float FRONT_WEIGHT_PERCENT = 0.45f; // 45%

    // Aero
    static constexpr float AIR_DENSITY = 1.225f;         // kg/m^3

    // Performance
    static constexpr float MAX_BRAKING_DECEL = 50.0f;    // m/s^2 (~5G)
    static constexpr float TIRE_GRIP_COEFFICIENT = 1.7f; // slicks dry peak

    // Braking
    static constexpr float MAX_BRAKE_PRESSURE = 100.0f;  // bar (scaled)

    // Derived
    static constexpr float GRAVITY = 9.81f;              // m/s^2
};

} // namespace Physics
} // namespace F1Game

