#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
namespace F1Game {
namespace Physics {
struct WheelSuspension {
    float springRate = 150000.0f;
    float damperRate = 8000.0f;
    float restLength = 0.10f;
    float travel = 0.05f;
    float compression = 0.0f;
    float velocity = 0.0f;
    float rideHeight = 0.03f;
    float load = 0.0f;
};
class Suspension {
public:
    Suspension();
    void Reset();
    void Update(double dt, float& chassisHeight, float& pitch, float& roll,
                float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR);
    WheelSuspension& GetWheel(int idx) { return m_wheels[idx]; }
    float GetRideHeight(int axle) const { return axle == 0 ? m_wheels[0].rideHeight : m_wheels[2].rideHeight; }
    float GetPitch() const { return m_pitch; }
    float GetRoll() const { return m_roll; }
    float GetChassisHeight() const { return m_chassisHeight; }
    void SetAntiRollStiffness(float front, float rear) { m_antiRollFront = front; m_antiRollRear = rear; }
private:
    void UpdateWheel(int idx, double dt, float normalLoad);
    void CalculateAntiRoll();
    WheelSuspension m_wheels[4];
    float m_chassisHeight = 0.035f;
    float m_pitch = 0.0f;
    float m_roll = 0.0f;
    float m_antiRollFront = 50000.0f;
    float m_antiRollRear = 40000.0f;
    float m_pitchStiffness = 80000.0f;
    float m_frontSpring = 150000.0f;
    float m_rearSpring = 120000.0f;
    float m_frontDamper = 8000.0f;
    float m_rearDamper = 8000.0f;
};
inline Suspension::Suspension() { Reset(); }
inline void Suspension::Reset() {
    m_wheels[0].springRate = m_frontSpring; m_wheels[0].damperRate = m_frontDamper; m_wheels[0].restLength = 0.10f;
    m_wheels[1].springRate = m_frontSpring; m_wheels[1].damperRate = m_frontDamper; m_wheels[1].restLength = 0.10f;
    m_wheels[2].springRate = m_rearSpring;  m_wheels[2].damperRate = m_rearDamper;  m_wheels[2].restLength = 0.10f;
    m_wheels[3].springRate = m_rearSpring;  m_wheels[3].damperRate = m_rearDamper;  m_wheels[3].restLength = 0.10f;
    for (int i = 0; i < 4; i++) { m_wheels[i].compression = 0; m_wheels[i].velocity = 0; m_wheels[i].rideHeight = 0.03f; m_wheels[i].load = 0; }
    m_chassisHeight = 0.035f; m_pitch = 0; m_roll = 0;
}
inline void Suspension::Update(double dt, float& chassisHeight, float& pitch, float& roll,
                               float normalLoadFL, float normalLoadFR, float normalLoadRL, float normalLoadRR) {
    UpdateWheel(0, dt, normalLoadFL);
    UpdateWheel(1, dt, normalLoadFR);
    UpdateWheel(2, dt, normalLoadRL);
    UpdateWheel(3, dt, normalLoadRR);
    CalculateAntiRoll();
    chassisHeight = m_chassisHeight;
    pitch = m_pitch;
    roll = m_roll;
}
inline void Suspension::UpdateWheel(int idx, double dt, float normalLoad) {
    float springForce = m_wheels[idx].springRate * m_wheels[idx].compression;
    float damperForce = m_wheels[idx].damperRate * m_wheels[idx].velocity;
    m_wheels[idx].load = springForce + damperForce + normalLoad;
    m_wheels[idx].rideHeight = m_wheels[idx].restLength - m_wheels[idx].compression;
    if (m_wheels[idx].rideHeight < 0.01f) m_wheels[idx].rideHeight = 0.01f;
    if (m_wheels[idx].rideHeight > 0.10f) m_wheels[idx].rideHeight = 0.10f;
    (void)dt;
}
inline void Suspension::CalculateAntiRoll() {
    float frontDiff = m_wheels[0].compression - m_wheels[1].compression;
    float rearDiff = m_wheels[2].compression - m_wheels[3].compression;
    m_roll = (frontDiff * m_antiRollFront + rearDiff * m_antiRollRear) * 0.001f;
    float frontPitch = ((m_wheels[0].compression + m_wheels[1].compression) - (m_wheels[2].compression + m_wheels[3].compression)) * 0.5f;
    m_pitch = frontPitch * 0.001f;
    float avgHeight = (m_wheels[0].rideHeight + m_wheels[1].rideHeight + m_wheels[2].rideHeight + m_wheels[3].rideHeight) * 0.25f;
    m_chassisHeight = avgHeight + 0.005f;
}
} // namespace Physics
} // namespace F1Game
