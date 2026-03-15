#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
namespace F1Game {
namespace Physics {
class Aerodynamics {
public:
    Aerodynamics();
    void Reset();
    void Update(float speedMPS, float rideHeight);
    void SetDRS(bool active) { m_drsActive = active; }
    void SetRearWingAngle(float angle);
    void SetFrontWingAngle(float angle);
    bool IsDRSActive() const { return m_drsActive; }
    float GetTotalDownforce() const { return m_totalDownforce; }
    float GetTotalDrag() const { return m_totalDrag; }
    float GetFrontAxleDownforce() const { return m_frontDownforce; }
    float GetRearAxleDownforce() const { return m_rearDownforce; }
    float GetDragForce() const { return m_dragForce; }
private:
    void CalculateForces(float speedMPS, float rideHeight);
    float m_totalDownforce = 0.0f;
    float m_totalDrag = 0.0f;
    float m_frontDownforce = 0.0f;
    float m_rearDownforce = 0.0f;
    float m_dragForce = 0.0f;
    float m_liftCoefficient = 4.0f;
    float m_dragCoefficient = 1.0f;
    float m_wingArea = 1.5f;
    float m_drsReduction = 1.0f;
    bool m_drsActive = false;
};
inline Aerodynamics::Aerodynamics() { Reset(); }
inline void Aerodynamics::Reset() {
    m_totalDownforce = 0;
    m_totalDrag = 0;
    m_frontDownforce = 0;
    m_rearDownforce = 0;
    m_dragForce = 0;
    m_drsActive = false;
    m_drsReduction = 1.0f;
}
inline void Aerodynamics::Update(float speedMPS, float rideHeight) { CalculateForces(speedMPS, rideHeight); }
inline void Aerodynamics::SetRearWingAngle(float angle) {
    m_liftCoefficient = 4.0f + angle * 0.05f;
    m_dragCoefficient = 1.0f + angle * 0.01f;
}
inline void Aerodynamics::SetFrontWingAngle(float angle) {
    (void)angle;
}
inline void Aerodynamics::CalculateForces(float speedMPS, float rideHeight) {
    float airDensity = F1Constants::AIR_DENSITY;
    float q = 0.5f * airDensity * speedMPS * speedMPS;
    float downforce = m_liftCoefficient * m_wingArea * q * m_drsReduction;
    float drag = m_dragCoefficient * m_wingArea * q;
    if (m_drsActive) {
        drag *= 0.90f;
        downforce *= 0.85f;
    }
    float groundEffect = 1.0f + (0.05f / fmaxf(rideHeight, 0.03f));
    downforce *= groundEffect;
    m_totalDownforce = downforce;
    m_totalDrag = drag;
    m_frontDownforce = downforce * 0.33f;
    m_rearDownforce = downforce * 0.67f;
    m_dragForce = drag;
}
} // namespace Physics
} // namespace F1Game
