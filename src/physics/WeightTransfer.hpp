#pragma once
#include <cmath>
#include "VehicleConstants.hpp"
namespace F1Game {
namespace Physics {
class WeightTransfer {
public:
    WeightTransfer();
    void Reset();
    void Update(double dt, float longitudinalAccel, float lateralAccel);
    float GetFrontAxleLoad() const { return m_frontAxleLoad; }
    float GetRearAxleLoad() const { return m_rearAxleLoad; }
    float GetFrontLeftLoad() const { return m_frontLeftLoad; }
    float GetFrontRightLoad() const { return m_frontRightLoad; }
    float GetRearLeftLoad() const { return m_rearLeftLoad; }
    float GetRearRightLoad() const { return m_rearRightLoad; }
    void SetBrakeBias(float bias) { m_brakeBias = bias; }
    void SetThrottleEffect(float effect) { m_throttleEffect = effect; }
private:
    void CalculateLongitudinalTransfer(float accel);
    void CalculateLateralTransfer(float lateralAccel);
    float m_frontAxleLoad = 0.0f;
    float m_rearAxleLoad = 0.0f;
    float m_frontLeftLoad = 0.0f;
    float m_frontRightLoad = 0.0f;
    float m_rearLeftLoad = 0.0f;
    float m_rearRightLoad = 0.0f;
    float m_brakeBias = 0.57f;
    float m_throttleEffect = 0.3f;
    float m_frontStiffness = 0.5f;
    float m_rearStiffness = 0.5f;
};
inline WeightTransfer::WeightTransfer() { Reset(); }
inline void WeightTransfer::Reset() {
    float totalWeight = F1Constants::MASS_NO_FUEL * F1Constants::GRAVITY;
    float rearBias = 1.0f - F1Constants::FRONT_WEIGHT_PERCENT;
    m_frontAxleLoad = totalWeight * rearBias;
    m_rearAxleLoad = totalWeight * F1Constants::FRONT_WEIGHT_PERCENT;
    m_frontLeftLoad = m_frontAxleLoad * 0.5f;
    m_frontRightLoad = m_frontAxleLoad * 0.5f;
    m_rearLeftLoad = m_rearAxleLoad * 0.5f;
    m_rearRightLoad = m_rearAxleLoad * 0.5f;
}
inline void WeightTransfer::Update(double dt, float longitudinalAccel, float lateralAccel) {
    (void)dt;
    CalculateLongitudinalTransfer(longitudinalAccel);
    CalculateLateralTransfer(lateralAccel);
}
inline void WeightTransfer::CalculateLongitudinalTransfer(float accel) {
    float mass = F1Constants::MASS_NO_FUEL;
    float g = F1Constants::GRAVITY;
    float cogHeight = F1Constants::COG_HEIGHT;
    float wheelbase = F1Constants::WHEELBASE;
    float rearBias = 1.0f - F1Constants::FRONT_WEIGHT_PERCENT;
    float frontBias = F1Constants::FRONT_WEIGHT_PERCENT;
    float totalWeight = mass * g;
    float deltaW = (mass * fabsf(accel) * cogHeight) / wheelbase;
    if (accel > 0) {
        m_frontAxleLoad = totalWeight * rearBias + deltaW * (1.0f - m_throttleEffect);
        m_rearAxleLoad = totalWeight * frontBias - deltaW * (1.0f - m_throttleEffect);
    } else if (accel < 0) {
        m_frontAxleLoad = totalWeight * rearBias + deltaW * m_brakeBias;
        m_rearAxleLoad = totalWeight * frontBias - deltaW * m_brakeBias;
    } else {
        m_frontAxleLoad = totalWeight * rearBias;
        m_rearAxleLoad = totalWeight * frontBias;
    }
    m_frontLeftLoad = m_frontAxleLoad * 0.5f;
    m_frontRightLoad = m_frontAxleLoad * 0.5f;
    m_rearLeftLoad = m_rearAxleLoad * 0.5f;
    m_rearRightLoad = m_rearAxleLoad * 0.5f;
}
inline void WeightTransfer::CalculateLateralTransfer(float lateralAccel) {
    float mass = F1Constants::MASS_NO_FUEL;
    float cogHeight = F1Constants::COG_HEIGHT;
    float trackFront = F1Constants::TRACK_WIDTH_FRONT;
    float trackRear = F1Constants::TRACK_WIDTH_REAR;
    float totalStiffness = m_frontStiffness + m_rearStiffness;
    float frontLatTransfer = (lateralAccel * mass * cogHeight * m_frontStiffness) / totalStiffness / trackFront;
    float rearLatTransfer = (lateralAccel * mass * cogHeight * m_rearStiffness) / totalStiffness / trackRear;
    m_frontRightLoad += frontLatTransfer;
    m_frontLeftLoad -= frontLatTransfer;
    m_rearRightLoad += rearLatTransfer;
    m_rearLeftLoad -= rearLatTransfer;
    m_frontLeftLoad = fmaxf(0.0f, m_frontLeftLoad);
    m_frontRightLoad = fmaxf(0.0f, m_frontRightLoad);
    m_rearLeftLoad = fmaxf(0.0f, m_rearLeftLoad);
    m_rearRightLoad = fmaxf(0.0f, m_rearRightLoad);
}
} // namespace Physics
} // namespace F1Game
