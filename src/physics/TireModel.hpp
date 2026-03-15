#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
#include "TireCompound.hpp"

namespace F1Game {
namespace Physics {

class Tire {
public:
    Tire();
    void Reset();
    void SetPressure(float psi);
    void SetCompound(const TireCompound* compound);
    void Update(float cornerLoad, float distanceKm, bool isWet, double dt);
    float CalculateLateralForce(float slipAngle, float normalLoad);
    float CalculateLongitudinalForce(float slipRatio, float normalLoad);
    void ApplyCombinedFriction(float& latForce, float& longForce, float maxLat, float maxLong);
    
    // Getters for physics/visuals
    float GetTemperature() const { return m_temperature; }
    float GetWear() const { return m_wear; }
    float GetPressure() const { return m_pressure; }
    float GetGripFactor() const { return m_gripFactor; }
    float GetSlipAngle() const { return m_slipAngle; }
    float GetSlipRatio() const { return m_slipRatio; }
    float GetGripMultiplier() const;  // Legacy
    const TireCompound* GetCompound() const { return m_compound; }
    float GetDegThisLap() const { return m_degThisLap; }
    void SetAmbientTemp(float temp) { m_ambientTemp = temp; }

private:
    float MagicFormula(float x, float B, float C, float D, float E);
    float clamp(float val, float min, float max) const { return val < min ? min : (val > max ? max : val); }

    const TireCompound* m_compound = nullptr;
    float m_temperature = 70.0f;   // °C
    float m_pressure = 22.0f;      // psi
    float m_wear = 0.0f;           // 0-100%
    float m_degThisLap = 0.0f;     // degradation this lap %
    float m_gripFactor = 1.0f;     // computed grip modifier
    float m_ambientTemp = 20.0f;
    float m_slipAngle = 0.0f;
    float m_slipRatio = 0.0f;
    float m_peakFriction = 1.7f;
};

// Inline implementations
inline Tire::Tire() { Reset(); }

inline void Tire::Reset() {
    m_temperature = 70.0f;
    m_wear = 0.0f;
    m_degThisLap = 0.0f;
    m_pressure = 22.0f;
    m_slipAngle = m_slipRatio = 0.0f;
    m_gripFactor = 1.0f;
}

inline void Tire::SetPressure(float psi) { 
    m_pressure = clamp(psi, 18.0f, 30.0f); 
}

inline void Tire::SetCompound(const TireCompound* compound) { 
    m_compound = compound; 
    m_peakFriction = compound ? compound->baseGrip : 1.0f;
}

inline void Tire::Update(float cornerLoad, float distanceKm, bool isWet, double dt) {
    if (!m_compound) return;

    // Thermal model
    float heatDelta = m_compound->heatGen * cornerLoad * (1.0f - m_wear * 0.003f);
    m_temperature = clamp(m_temperature + heatDelta - 0.8f * (float)dt * 10.0f, 20.0f, 140.0f);

    // Wear model (km-based)
    float tempMultiplier = (m_temperature > 110.0f) ? 2.2f : 1.0f;
    float wearDelta = m_compound->wearRate * distanceKm * tempMultiplier;
    m_degThisLap += wearDelta;
    m_wear = clamp(m_wear + wearDelta, 0.0f, 100.0f);

    // Grip factor (core formula)
    float wearMult = 1.0f - m_wear * 0.0065f;
    float tempMult = 1.0f - fabsf(m_temperature - m_compound->optimalTemp) * 0.008f;
    float wetMult = isWet ? (m_compound->type == TireType::WET ? 1.0f : 0.4f) : 1.0f;
    m_gripFactor = wearMult * tempMult * wetMult;
}

inline float Tire::GetGripMultiplier() const {
    return m_gripFactor * (m_peakFriction / 1.7f);
}

inline float Tire::MagicFormula(float x, float B, float C, float D, float E) {
    return D * sinf(C * atanf(B * x - E * (B * x - atanf(B * x))));
}

inline float Tire::CalculateLateralForce(float slipAngle, float normalLoad) {
    m_slipAngle = slipAngle;
    float B = 11.0f, C = 1.9f, E = 0.97f;
    float D = normalLoad * m_peakFriction * m_gripFactor;
    float x = slipAngle * 0.0174533f;  // deg to rad
    return MagicFormula(x, B, C, D, E);
}

inline float Tire::CalculateLongitudinalForce(float slipRatio, float normalLoad) {
    m_slipRatio = slipRatio;
    float B = 10.0f, C = 1.65f, E = 0.97f;
    float D = normalLoad * m_peakFriction * m_gripFactor;
    return MagicFormula(slipRatio, B, C, D, E);
}

inline void Tire::ApplyCombinedFriction(float& latForce, float& longForce, float maxLat, float maxLong) {
    if (maxLat <= 0 || maxLong <= 0) return;
    float combined = sqrtf((longForce / maxLong)*(longForce / maxLong) + (latForce / maxLat)*(latForce / maxLat));
    if (combined > 1.0f) {
        latForce /= combined;
        longForce /= combined;
    }
}

typedef Tire TireModel;  // Backward compat for VehicleBody

} // namespace Physics
} // F1Game

