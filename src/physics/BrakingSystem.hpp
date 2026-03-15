#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
namespace F1Game {
namespace Physics {
class BrakingSystem {
public:
    BrakingSystem();
    void Reset();
    void SetBrakePedal(float pedal);
    void SetBrakeBias(float bias);
    void Update(double dt, float speedMPS, float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR);
    float GetFrontBrakeForce() const { return m_frontBrakeForce; }
    float GetRearBrakeForce() const { return m_rearBrakeForce; }
    float GetFrontLeftBrake() const { return m_frontLeftBrakeForce; }
    float GetFrontRightBrake() const { return m_frontRightBrakeForce; }
    float GetRearLeftBrake() const { return m_rearLeftBrakeForce; }
    float GetRearRightBrake() const { return m_rearRightBrakeForce; }
    float GetTotalBrakeForce() const { return m_totalBrakeForce; }
    float GetDeceleration() const { return m_deceleration; }
    bool IsABSActive() const { return m_absActive; }
    bool IsBraking() const { return m_brakePedal > 0.01f; }
    void SetABS(bool enabled) { m_absEnabled = enabled; }
    void SetEBS(bool enabled) { m_ebsEnabled = enabled; }
    float GetFrontLeftSlip() const { return m_slipFL; }
    float GetFrontRightSlip() const { return m_slipFR; }
    float GetRearLeftSlip() const { return m_slipRL; }
    float GetRearRightSlip() const { return m_slipRR; }
    bool IsFrontLeftLocked() const { return m_lockedFL; }
    bool IsFrontRightLocked() const { return m_lockedFR; }
    bool IsRearLeftLocked() const { return m_lockedRL; }
    bool IsRearRightLocked() const { return m_lockedRR; }
    float GetTemperature() const { return m_temperature; }
private:
    void CalculateBrakeForces(float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR);
    void UpdateTemperature(double dt, float speedMPS);
    float GetBrakePerformanceMultiplier();
    float m_brakePedal = 0.0f;
    float m_brakeBias = 0.57f;
    float m_frontBrakeForce = 0.0f;
    float m_rearBrakeForce = 0.0f;
    float m_frontLeftBrakeForce = 0.0f;
    float m_frontRightBrakeForce = 0.0f;
    float m_rearLeftBrakeForce = 0.0f;
    float m_rearRightBrakeForce = 0.0f;
    float m_totalBrakeForce = 0.0f;
    float m_deceleration = 0.0f;
    bool m_absActive = false;
    bool m_absEnabled = true;
    bool m_ebsEnabled = false;
    float m_slipFL = 0.0f;
    float m_slipFR = 0.0f;
    float m_slipRL = 0.0f;
    float m_slipRR = 0.0f;
    bool m_lockedFL = false;
    bool m_lockedFR = false;
    bool m_lockedRL = false;
    bool m_lockedRR = false;
    float m_temperature = 200.0f;
    float m_absThreshold = 0.95f;
};
inline BrakingSystem::BrakingSystem() { Reset(); }
inline void BrakingSystem::Reset() {
    m_brakePedal = 0;
    m_frontBrakeForce = 0;
    m_rearBrakeForce = 0;
    m_totalBrakeForce = 0;
    m_deceleration = 0;
    m_absActive = false;
    m_temperature = 200.0f;
    m_lockedFL = m_lockedFR = m_lockedRL = m_lockedRR = false;
}
inline void BrakingSystem::SetBrakePedal(float pedal) { m_brakePedal = pedal > 1 ? 1 : (pedal < 0 ? 0 : pedal); }
inline void BrakingSystem::SetBrakeBias(float bias) { m_brakeBias = bias > 0.7f ? 0.7f : (bias < 0.4f ? 0.4f : bias); }
inline void BrakingSystem::Update(double dt, float speedMPS, float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR) {
    CalculateBrakeForces(normalLoadFL, normalLoadFR, normalLoadRL, normalLoadRR);
    UpdateTemperature(dt, speedMPS);
    m_deceleration = m_totalBrakeForce / F1Constants::MASS_NO_FUEL;
    if (m_deceleration > F1Constants::MAX_BRAKING_DECEL) m_deceleration = F1Constants::MAX_BRAKING_DECEL;
    if (speedMPS < 0.5f) m_deceleration *= speedMPS / 0.5f;
}
inline void BrakingSystem::CalculateBrakeForces(float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR) {
    float maxPressure = F1Constants::MAX_BRAKE_PRESSURE;
    if (m_ebsEnabled) maxPressure *= 1.2f;
    float perfMult = GetBrakePerformanceMultiplier();
    float frontBase = m_brakePedal * maxPressure * m_brakeBias * perfMult;
    float rearBase = m_brakePedal * maxPressure * (1.0f - m_brakeBias) * perfMult;
    float frictionCoeff = F1Constants::TIRE_GRIP_COEFFICIENT;
    float maxFrontGrip = frictionCoeff * (normalLoadFL + normalLoadFR);
    float maxRearGrip = frictionCoeff * (normalLoadRL + normalLoadRR);
    float frontForce = frontBase * 1000.0f;
    float rearForce = rearBase * 1000.0f;
    m_absActive = false;
    if (m_absEnabled) {
        if (frontForce > maxFrontGrip * m_absThreshold) { frontForce = maxFrontGrip * m_absThreshold; m_absActive = true; }
        if (rearForce > maxRearGrip * m_absThreshold) { rearForce = maxRearGrip * m_absThreshold; m_absActive = true; }
    }
    m_frontLeftBrakeForce = frontForce * 0.5f;
    m_frontRightBrakeForce = frontForce * 0.5f;
    m_rearLeftBrakeForce = rearForce * 0.5f;
    m_rearRightBrakeForce = rearForce * 0.5f;
    float maxFL = frictionCoeff * normalLoadFL;
    float maxFR = frictionCoeff * normalLoadFR;
    float maxRL = frictionCoeff * normalLoadRL;
    float maxRR = frictionCoeff * normalLoadRR;
    if (m_frontLeftBrakeForce > maxFL) { m_lockedFL = true; m_slipFL = -1.0f; m_frontLeftBrakeForce = maxFL; }
    else { m_lockedFL = false; m_slipFL = m_frontLeftBrakeForce / maxFL; }
    if (m_frontRightBrakeForce > maxFR) { m_lockedFR = true; m_slipFR = -1.0f; m_frontRightBrakeForce = maxFR; }
    else { m_lockedFR = false; m_slipFR = m_frontRightBrakeForce / maxFR; }
    if (m_rearLeftBrakeForce > maxRL) { m_lockedRL = true; m_slipRL = -1.0f; m_rearLeftBrakeForce = maxRL; }
    else { m_lockedRL = false; m_slipRL = m_rearLeftBrakeForce / maxRL; }
    if (m_rearRightBrakeForce > maxRR) { m_lockedRR = true; m_slipRR = -1.0f; m_rearRightBrakeForce = maxRR; }
    else { m_lockedRR = false; m_slipRR = m_rearRightBrakeForce / maxRR; }
    m_frontBrakeForce = frontForce;
    m_rearBrakeForce = rearForce;
    m_totalBrakeForce = m_frontLeftBrakeForce + m_frontRightBrakeForce + m_rearLeftBrakeForce + m_rearRightBrakeForce;
}
inline void BrakingSystem::UpdateTemperature(double dt, float speedMPS) {
    float heatInput = m_brakePedal * speedMPS * 0.5f;
    float heatLoss = (m_temperature - 20.0f) * 0.01f;
    m_temperature += (heatInput - heatLoss) * (float)dt;
    if (m_temperature < 20) m_temperature = 20;
}
inline float BrakingSystem::GetBrakePerformanceMultiplier() {
    if (m_temperature < 200) return 0.3f + (m_temperature - 20.0f) / 180.0f * 0.2f;
    if (m_temperature < 400) return 0.5f + (m_temperature - 200.0f) / 200.0f * 0.3f;
    if (m_temperature <= 1000) return 1.0f;
    if (m_temperature <= 1200) return 1.0f - (m_temperature - 1000.0f) / 200.0f * 0.15f;
    return 0.5f;
}
} // namespace Physics
} // namespace F1Game
