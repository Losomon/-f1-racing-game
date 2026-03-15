#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
namespace F1Game {
namespace Physics {
class EngineDrivetrain {
public:
    EngineDrivetrain();
    void Reset();
    void SetThrottle(float throttle);
    void SetBrake(float brake);
    void SetClutch(float clutch);
    void ShiftUp();
    void ShiftDown();
    void Update(double dt, float wheelAngularVelocity, float vehicleSpeedMPS);
    float GetEngineRPM() const { return m_currentRPM; }
    float GetThrottlePosition() const { return m_throttle; }
    float GetBrakePosition() const { return m_brake; }
    int GetCurrentGear() const { return m_currentGear; }
    float GetDriveTorque() const { return m_driveTorque; }
    float GetEngineTorque() const { return m_engineTorque; }
    float GetWheelTorque() const { return m_wheelTorque; }
    float GetDriveForce() const { return m_driveForce; }
    bool IsUpshifting() const { return m_isShifting && m_shiftDirection > 0; }
    bool IsDownshifting() const { return m_isShifting && m_shiftDirection < 0; }
    bool IsWheelspinning() const { return m_wheelspin; }
    void SetTractionControl(bool enabled) { m_tractionControlEnabled = enabled; }
    float GetSlipRatio() const { return m_slipRatio; }
private:
    float GetEngineTorqueCurve(float rpm);
    void UpdateGearRatios();
    void DetectWheelspin(float wheelAngularVelocity, float vehicleSpeedMPS);
    float m_throttle = 0.0f;
    float m_brake = 0.0f;
    float m_clutch = 0.0f;
    int m_currentGear = 1;
    int m_targetGear = 1;
    float m_currentRPM = 3000.0f;
    float m_engineTorque = 0.0f;
    float m_driveTorque = 0.0f;
    float m_wheelTorque = 0.0f;
    float m_driveForce = 0.0f;
    float m_shiftTimer = 0.0f;
    bool m_isShifting = false;
    int m_shiftDirection = 0;
    bool m_wheelspin = false;
    bool m_tractionControlEnabled = true;
    float m_slipRatio = 0.0f;
    static constexpr float m_gearRatios[9] = {0.0f, 2.94f, 2.18f, 1.72f, 1.39f, 1.14f, 0.95f, 0.81f, 0.70f};
    static constexpr float m_finalDrive = 3.5f;
    static constexpr float m_shiftTime = 0.2f;
    static constexpr float m_wheelRadius = 0.33f;
    static constexpr float m_tcThreshold = 0.15f;
};
inline EngineDrivetrain::EngineDrivetrain() { Reset(); }
inline void EngineDrivetrain::Reset() {
    m_currentRPM = 3000;
    m_currentGear = 1;
    m_targetGear = 1;
    m_throttle = 0;
    m_brake = 0;
    m_clutch = 0;
    m_isShifting = false;
    m_wheelspin = false;
    m_slipRatio = 0;
}
inline void EngineDrivetrain::SetThrottle(float t) { m_throttle = t > 1 ? 1 : (t < 0 ? 0 : t); }
inline void EngineDrivetrain::SetBrake(float b) { m_brake = b > 1 ? 1 : (b < 0 ? 0 : b); }
inline void EngineDrivetrain::SetClutch(float c) { m_clutch = c > 1 ? 1 : (c < 0 ? 0 : c); }
inline void EngineDrivetrain::ShiftUp() { if (m_currentGear < 8) { m_targetGear = m_currentGear + 1; m_isShifting = true; m_shiftDirection = 1; m_shiftTimer = m_shiftTime; } }
inline void EngineDrivetrain::ShiftDown() { if (m_currentGear > 1) { m_targetGear = m_currentGear - 1; m_isShifting = true; m_shiftDirection = -1; m_shiftTimer = m_shiftTime; } }
inline void EngineDrivetrain::Update(double dt, float wheelAngularVelocity, float vehicleSpeedMPS) {
    if (m_isShifting) {
        m_shiftTimer -= dt;
        if (m_shiftTimer <= 0) { m_currentGear = m_targetGear; m_isShifting = false; m_shiftDirection = 0; }
    }
    float gearRatio = m_gearRatios[m_currentGear];
    float finalRatio = gearRatio * m_finalDrive;
    float wheelRPM = (vehicleSpeedMPS / m_wheelRadius) * 60.0f / 6.28318f;
    if (finalRatio > 0) m_currentRPM = wheelRPM * finalRatio * (1.0f - m_clutch * 0.8f) + m_throttle * 2000.0f;
    m_currentRPM = m_currentRPM > 15000 ? 15000 : (m_currentRPM < 3000 ? 3000 : m_currentRPM);
    m_engineTorque = GetEngineTorqueCurve(m_currentRPM);
    m_driveTorque = m_throttle * m_engineTorque * gearRatio * m_finalDrive * (1.0f - m_clutch * 0.8f);
    m_wheelTorque = m_driveTorque / 2.0f;
    m_driveForce = m_driveTorque / m_wheelRadius;
    DetectWheelspin(wheelAngularVelocity, vehicleSpeedMPS);
}
inline float EngineDrivetrain::GetEngineTorqueCurve(float rpm) {
    float normalizedRPM = (rpm - 3000.0f) / 12000.0f;
    float peak = 510.0f;
    float shape = sinf(normalizedRPM * 3.14159f) * 0.5f + 0.5f;
    return fmaxf(200.0f, peak * (0.6f + 0.4f * shape));
}
inline void EngineDrivetrain::DetectWheelspin(float wheelAngularVelocity, float vehicleSpeedMPS) {
    float expectedAngularVel = vehicleSpeedMPS / m_wheelRadius;
    m_slipRatio = 0;
    m_wheelspin = false;
    if (expectedAngularVel > 0.01f) {
        m_slipRatio = (wheelAngularVelocity - expectedAngularVel) / expectedAngularVel;
        if (m_slipRatio > m_tcThreshold) {
            m_wheelspin = true;
            if (m_tractionControlEnabled) {
                m_throttle *= 0.85f;
            }
        }
    }
}
inline void EngineDrivetrain::UpdateGearRatios() {}
} // namespace Physics
} // namespace F1Game
