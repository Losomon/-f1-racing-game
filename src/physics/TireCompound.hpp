#pragma once
#include <raylib.h>

namespace F1Game {
namespace Physics {

enum class TireType { SOFT, MEDIUM, HARD, INTER, WET };

struct TireCompound {
    TireType type;
    float baseGrip;      // relative to soft=1.0
    float wearRate;      // % per km
    float heatGen;       // °C per unit corner load
    float optimalTemp;   // °C
    Color color;         // sidewall visual
};

// Predefined 2026 F1 compounds (data-driven)
inline const TireCompound SOFT_COMPOUND = {
    TireType::SOFT, 1.00f, 0.025f, 1.2f, 92.0f, {255, 0, 0, 255}  // Red
};

inline const TireCompound MEDIUM_COMPOUND = {
    TireType::MEDIUM, 0.95f, 0.018f, 1.0f, 90.0f, {255, 127, 0, 255}  // Yellow
};

inline const TireCompound HARD_COMPOUND = {
    TireType::HARD, 0.90f, 0.012f, 0.8f, 88.0f, {255, 255, 255, 255}  // White
};

inline const TireCompound INTER_COMPOUND = {
    TireType::INTER, 0.75f, 0.020f, 0.9f, 75.0f, {0, 191, 255, 255}  // Light Blue
};

inline const TireCompound WET_COMPOUND = {
    TireType::WET, 0.60f, 0.015f, 0.7f, 70.0f, {0, 0, 255, 255}  // Blue
};

} // Physics
} // F1Game

