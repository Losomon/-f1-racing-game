#pragma once
#include <cmath>
#include <raylib.h>
#include "F1Constants.hpp"
#include "TireModel.hpp"
#include "Aerodynamics.hpp"
#include "EngineDrivetrain.hpp"
#include "BrakingSystem.hpp"
#include "WeightTransfer.hpp"
#include "Suspension.hpp"

namespace F1Game {
namespace Physics {

constexpr float WHEELBASE = 3.6f;
constexpr float MASS_NO_FUEL = 798.0f;
constexpr float TIRE_GRIP_COEFFICIENT = 1.6f;

struct VehicleInput {
    float throttle = 0.0f;
    float brake    = 0.0f;
    float steering = 0.0f;
};

struct VehicleState {
    float   speedKmh = 0.0f;
    float   posX     = 0.0f;
    float   posZ     = 0.0f;
    float   heading  = 0.0f;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    float   yaw      = 0.0f;
};

// Safe clamp helper
static inline float Clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
// Sanitize: replace NaN/Inf with fallback
static inline float Safe(float v, float fallback = 0.0f) {
    return (std::isfinite(v)) ? v : fallback;
}

class VehicleBody {
public:
    VehicleBody() { Reset(); }

    void Reset() {
        m_positionX = 0.0f;
        m_positionZ = 0.0f;
        m_velocityX = m_velocityZ = 0.0f;
        m_speedMPS = m_angularVelocity = m_lateralVelocity = 0.0f;
        m_heading = 0.0f;
        m_throttle = m_brake = m_steeringAngle = 0.0f;
        m_drsActive = false;
        // Set default medium compound so tire grip/wear model works
        static TireCompound defaultCompound;
        defaultCompound.type        = TireType::MEDIUM;
        defaultCompound.baseGrip    = 1.7f;
        defaultCompound.optimalTemp = 90.0f;
        defaultCompound.heatGen     = 0.02f;
        defaultCompound.wearRate    = 0.005f;
        for (int i = 0; i < 4; i++) {
            m_tires[i].Reset();
            m_tires[i].SetCompound(&defaultCompound);
            m_slipRatios[i] = m_slipAngles[i] = m_normalLoads[i] = 0.0f;
            m_lateralForces[i] = m_longitudinalForces[i] = 0.0f;
        }
        m_aerodynamics.Reset();
        m_drivetrain.Reset();
        m_braking.Reset();
        m_weightTransfer.Reset();
        m_suspension.Reset();
    }

    void SetInputs(float throttle, float brake, float steering) {
        m_throttle     = Clamp(throttle, 0.0f, 1.0f);
        m_brake        = Clamp(brake,    0.0f, 1.0f);
        m_steeringAngle = Clamp(steering, -1.0f, 1.0f);
    }

    void SetInput(const VehicleInput& input) {
        SetInputs(input.throttle, input.brake, input.steering);
        m_input = input;
    }

    VehicleState GetState() const {
        VehicleState s;
        s.speedKmh = Safe(GetSpeedKPH());
        s.posX     = Safe(m_positionX);
        s.posZ     = Safe(m_positionZ);
        s.heading  = Safe(m_heading);
        s.position = {Safe(m_positionX), 0.0f, Safe(m_positionZ)};
        s.yaw      = Safe(m_heading);
        return s;
    }

    void Update(double dt) {
        const float fdt = (float)dt;
        const float mass = F1Constants::MASS_NO_FUEL;

        m_drivetrain.SetThrottle(m_throttle);
        m_drivetrain.SetBrake(m_brake);

        // CRITICAL: must call Update() or GetDriveForce() always returns 0
        m_drivetrain.Update(fdt, m_speedMPS / 0.33f, m_speedMPS);

        CalculateSlipRatiosAndAngles();
        m_aerodynamics.Update(m_speedMPS, m_chassisHeight);
        CalculateNormalLoads();

        // CRITICAL: must call braking Update() or GetTotalBrakeForce() always returns 0
        m_braking.SetBrakePedal(m_brake);
        m_braking.Update(fdt, m_speedMPS,
            m_normalLoads[0], m_normalLoads[1],
            m_normalLoads[2], m_normalLoads[3]);

        CalculateTireForces();

        float driveForce  = Safe(m_drivetrain.GetDriveForce());
        float brakeForce  = Safe(m_braking.GetTotalBrakeForce());
        float dragForce   = Safe(m_aerodynamics.GetDragForce());

        // Hard cap forces to prevent runaway
        driveForce = Clamp(driveForce, 0.0f, 15000.0f);
        brakeForce = Clamp(brakeForce, 0.0f, 15000.0f);
        dragForce  = Clamp(dragForce,  0.0f, 10000.0f);

        float cosH = cosf(m_heading);
        float sinH = sinf(m_heading);

        // Net longitudinal force in world space
        float longNet = driveForce - brakeForce - dragForce;
        longNet = Clamp(longNet, -15000.0f, 15000.0f);

        // Simple lateral from steering (no tire blow-up)
        float latNet = Clamp(-m_steeringAngle * m_speedMPS * mass * 0.3f, -8000.0f, 8000.0f);

        // Forward is +Z, right is +X in raylib
        // cosH/sinH: heading=0 means facing +Z
        float ax = (longNet * sinH + latNet * cosH) / mass;
        float az = (longNet * cosH - latNet * sinH) / mass;

        // Cap acceleration to ~3g
        ax = Clamp(ax, -30.0f, 30.0f);
        az = Clamp(az, -30.0f, 30.0f);

        m_velocityX += ax * fdt;
        m_velocityZ += az * fdt;

        // Cap speed to 100 m/s (~360 km/h)
        float speed = sqrtf(m_velocityX * m_velocityX + m_velocityZ * m_velocityZ);
        if (speed > 100.0f) {
            float scale = 100.0f / speed;
            m_velocityX *= scale;
            m_velocityZ *= scale;
        }

        // Apply drag friction when no throttle
        float friction = (m_throttle < 0.01f) ? 0.98f : 0.9995f;
        m_velocityX *= friction;
        m_velocityZ *= friction;

        // Steering yaw rate
        m_angularVelocity = m_steeringAngle * (m_speedMPS / 20.0f) * 1.2f;
        m_angularVelocity = Clamp(m_angularVelocity, -2.0f, 2.0f);
        m_heading += m_angularVelocity * fdt;

        m_speedMPS  = Safe(sqrtf(m_velocityX*m_velocityX + m_velocityZ*m_velocityZ));
        m_positionX = Safe(m_positionX + m_velocityX * fdt);
        m_positionZ = Safe(m_positionZ + m_velocityZ * fdt);

        // Sanity check - if anything goes NaN reset velocity
        if (!std::isfinite(m_positionX) || !std::isfinite(m_positionZ)) {
            m_positionX = m_positionZ = 0.0f;
            m_velocityX = m_velocityZ = 0.0f;
            m_speedMPS  = 0.0f;
        }

        m_suspension.Update(fdt, m_chassisHeight, m_pitch, m_roll,
            m_normalLoads[0], m_normalLoads[1], m_normalLoads[2], m_normalLoads[3]);

        float distKm = m_speedMPS * fdt / 1000.0f;
        bool isWet = false;
        float cornerLoad = fabsf(m_slipRatios[0]) + fabsf(m_slipAngles[0]);
        for (int i = 0; i < 4; i++) {
            m_tires[i].Update(cornerLoad, distKm, isWet, fdt);
            m_slipAngles[i] = m_tires[i].GetSlipAngle();
            m_slipRatios[i] = m_tires[i].GetSlipRatio();
        }
    }

    float GetSpeedMPS() const { return m_speedMPS; }
    float GetSpeedKPH() const { return m_speedMPS * 3.6f; }
    float GetHeading()  const { return m_heading; }
    float GetPositionX() const { return m_positionX; }
    float GetPositionZ() const { return m_positionZ; }
    float GetEngineRPM()   const { return m_drivetrain.GetEngineRPM(); }
    int   GetCurrentGear() const { return m_drivetrain.GetCurrentGear(); }
    float GetSteeringAngle() const { return m_steeringAngle; }
    bool  IsDRSActive() const { return m_drsActive; }
    void  SetDRS(bool active) { m_drsActive = active; m_aerodynamics.SetDRS(active); }
    float GetSlipAngleFL()  const { return m_slipAngles[0]; }
    float GetSlipAngleFR()  const { return m_slipAngles[1]; }
    float GetNormalLoadFL() const { return m_normalLoads[0]; }
    float GetNormalLoadFR() const { return m_normalLoads[1]; }
    float GetDownforce()    const { return m_aerodynamics.GetTotalDownforce(); }
    float GetDrag()         const { return m_aerodynamics.GetTotalDrag(); }
    float GetTireTempFL()   const { return m_tires[0].GetTemperature(); }
    float GetTireWearFL()   const { return m_tires[0].GetWear(); }
    const Tire& GetTireFL() const { return m_tires[0]; }
    float GetGripFactorFL() const { return m_tires[0].GetGripFactor(); }

private:
    void CalculateSlipRatiosAndAngles() {
        float wheelRadius = F1Constants::WHEEL_RADIUS;
        float cosH = cosf(m_heading), sinH = sinf(m_heading);
        float forwardVel = m_velocityX * cosH + m_velocityZ * sinH;
        m_lateralVelocity = -m_velocityX * sinH + m_velocityZ * cosH;
        float yawRate = m_angularVelocity;
        float frontAxleX =  F1Constants::WHEELBASE * 0.5f;
        float rearAxleX  = -F1Constants::WHEELBASE * 0.5f;
        for (int i = 0; i < 4; i++) {
            float axleX = (i < 2) ? frontAxleX : rearAxleX;
            float wheelSpeed = (forwardVel + yawRate * axleX) / wheelRadius;
            if (m_speedMPS > 0.1f) {
                float ref = fmaxf(m_speedMPS / wheelRadius, 1.0f);
                m_slipRatios[i] = Clamp((wheelSpeed - m_speedMPS / wheelRadius) / ref, -1.0f, 1.0f);
            } else {
                m_slipRatios[i] = 0.0f;
            }
            float slipAngle = (i < 2)
                ? atan2f(m_lateralVelocity + yawRate * frontAxleX, fmaxf(m_speedMPS, 0.1f)) - m_steeringAngle * 0.5f
                : atan2f(m_lateralVelocity + yawRate * rearAxleX,  fmaxf(m_speedMPS, 0.1f));
            m_slipAngles[i] = Safe(slipAngle * 57.2958f);
        }
    }

    void CalculateNormalLoads() {
        float frontDown = Safe(m_aerodynamics.GetFrontAxleDownforce());
        float rearDown  = Safe(m_aerodynamics.GetRearAxleDownforce());
        float accel = Safe((m_drivetrain.GetDriveForce() - m_braking.GetTotalBrakeForce()) / F1Constants::MASS_NO_FUEL);
        float latAccel = Safe(m_angularVelocity * m_speedMPS);
        m_weightTransfer.Update(0.001, accel, latAccel);
        float frontBase = Safe(m_weightTransfer.GetFrontAxleLoad()) + frontDown;
        float rearBase  = Safe(m_weightTransfer.GetRearAxleLoad())  + rearDown;
        // Minimum load to avoid zero divide in tire model
        frontBase = fmaxf(frontBase, 100.0f);
        rearBase  = fmaxf(rearBase,  100.0f);
        m_normalLoads[0] = m_normalLoads[1] = frontBase * 0.5f;
        m_normalLoads[2] = m_normalLoads[3] = rearBase  * 0.5f;
    }

    void CalculateTireForces() {
        for (int i = 0; i < 4; i++) {
            float latForce  = Safe(m_tires[i].CalculateLateralForce(m_slipAngles[i], m_normalLoads[i]));
            float longForce = Safe(m_tires[i].CalculateLongitudinalForce(m_slipRatios[i], m_normalLoads[i]));
            float maxLat    = m_normalLoads[i] * F1Constants::TIRE_GRIP_COEFFICIENT;
            float maxLong   = m_normalLoads[i] * F1Constants::TIRE_GRIP_COEFFICIENT;
            m_tires[i].ApplyCombinedFriction(latForce, longForce, maxLat, maxLong);
            m_lateralForces[i]      = Clamp(Safe(latForce),  -maxLat,  maxLat);
            m_longitudinalForces[i] = Clamp(Safe(longForce), -maxLong, maxLong);
        }
    }

    float m_positionX = 0.0f, m_positionZ = 0.0f;
    float m_velocityX = 0.0f, m_velocityZ = 0.0f;
    float m_speedMPS  = 0.0f, m_heading   = 0.0f;
    float m_angularVelocity = 0.0f, m_lateralVelocity = 0.0f;
    float m_throttle = 0.0f, m_brake = 0.0f, m_steeringAngle = 0.0f;
    float m_slipRatios[4]        = {0};
    float m_slipAngles[4]        = {0};
    float m_normalLoads[4]       = {0};
    float m_lateralForces[4]     = {0};
    float m_longitudinalForces[4]= {0};
    Tire             m_tires[4];
    Aerodynamics     m_aerodynamics;
    EngineDrivetrain m_drivetrain;
    BrakingSystem    m_braking;
    WeightTransfer   m_weightTransfer;
    Suspension       m_suspension;
    VehicleInput     m_input;
    bool  m_drsActive    = false;
    float m_chassisHeight = 0.035f, m_pitch = 0.0f, m_roll = 0.0f;
};

} // namespace Physics
} // namespace F1Game