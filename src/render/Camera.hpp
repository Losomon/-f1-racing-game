#pragma once

#include <raylib.h>
#include "Renderer.hpp"

namespace F1Game {
namespace render {

// Camera modes
enum class CameraMode {
    Follow,       // Follow a target
    Cinematic,     // Cinematic camera
    Orbit,         // Orbit around target
    Free           // Free fly camera
};

// Follow camera configuration
struct FollowConfig {
    Vector3 offset = {0, 5, -10};    // Offset from target
    float lookAhead = 5.0f;          // Look ahead distance
    float smoothing = 5.0f;           // Position smoothing
    float rotationSmoothing = 3.0f; // Rotation smoothing
    float minDistance = 3.0f;
    float maxDistance = 20.0f;
};

// Implementation of ICamera - Follow camera for racing
class Camera : public ICamera {
public:
    Camera();
    ~Camera() = default;

    // Configuration
    void SetMode(CameraMode mode);
    void SetTarget(Entity target);
    void SetFollowConfig(const FollowConfig& config);
    void SetFOV(float fov);
    
    // ICamera interface
    void Update(double dt) override;
    Matrix GetViewMatrix() const override;
    Matrix GetProjectionMatrix() const override;
    Vector3 GetPosition() const override { return m_position; }
    Vector3 GetTarget() const override { return m_target; }

    // Get internal camera for Raylib
    Camera3D GetRaylibCamera() const;

private:
    CameraMode m_mode = CameraMode::Follow;
    Entity m_targetEntity = 0;
    
    Vector3 m_position = {0, 5, -10};
    Vector3 m_target = {0, 0, 0};
    Vector3 m_velocity = {0, 0, 0};
    
    FollowConfig m_followConfig;
    float m_fov = 60.0f;
    
    // Smoothing
    Vector3 m_smoothedPosition;
    Vector3 m_smoothedTarget;
    
    void UpdateFollowMode(double dt);
    void UpdateCinematicMode(double dt);
    void UpdateOrbitMode(double dt);
    void UpdateFreeMode(double dt);
};

// Inline implementations
inline Camera::Camera() {
    m_smoothedPosition = m_position;
    m_smoothedTarget = m_target;
}

inline void Camera::SetMode(CameraMode mode) {
    m_mode = mode;
}

inline void Camera::SetTarget(Entity target) {
    m_targetEntity = target;
}

inline void Camera::SetFollowConfig(const FollowConfig& config) {
    m_followConfig = config;
}

inline void Camera::SetFOV(float fov) {
    m_fov = fov;
}

inline void Camera::Update(double dt) {
    switch (m_mode) {
        case CameraMode::Follow:
            UpdateFollowMode(dt);
            break;
        case CameraMode::Cinematic:
            UpdateCinematicMode(dt);
            break;
        case CameraMode::Orbit:
            UpdateOrbitMode(dt);
            break;
        case CameraMode::Free:
            UpdateFreeMode(dt);
            break;
    }
}

inline Matrix Camera::GetViewMatrix() const {
    return MatrixLookAt(m_smoothedPosition, m_smoothedTarget, {0, 1, 0});
}

inline Matrix Camera::GetProjectionMatrix() const {
    return MatrixPerspective(m_fov * DEG2RAD, (float)GetScreenWidth() / GetScreenHeight(), 0.1f, 1000.0f);
}

inline Camera3D Camera::GetRaylibCamera() const {
    Camera3D cam;
    cam.position = m_smoothedPosition;
    cam.target = m_smoothedTarget;
    cam.up = {0, 1, 0};
    cam.fovy = m_fov;
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}

inline void Camera::UpdateFollowMode(double dt) {
    float deltaTime = (float)dt;
    
    if (m_targetEntity != 0 && g_engine && g_engine->ecs) {
        auto* transform = g_engine->ecs->GetTransform(m_targetEntity);
        if (transform) {
            // Target position from entity
            Vector3 targetPos = transform->position;
            float rotation = transform->rotation.z;
            
            // Calculate desired camera position based on offset
            float sinRot = sinf(rotation * DEG2RAD);
            float cosRot = cosf(rotation * DEG2RAD);
            
            Vector3 desiredPos = {
                targetPos.x + m_followConfig.offset.x * cosRot - m_followConfig.offset.z * sinRot,
                targetPos.y + m_followConfig.offset.y,
                targetPos.z + m_followConfig.offset.x * sinRot + m_followConfig.offset.z * cosRot
            };
            
            // Look ahead target
            Vector3 desiredTarget = {
                targetPos.x + m_followConfig.lookAhead * cosRot,
                targetPos.y,
                targetPos.z + m_followConfig.lookAhead * sinRot
            };
            
            // Smooth position
            float t = 1.0f - expf(-m_followConfig.smoothing * deltaTime);
            m_smoothedPosition = Vector3Lerp(m_smoothedPosition, desiredPos, t);
            m_smoothedTarget = Vector3Lerp(m_smoothedTarget, desiredTarget, t);
            
            m_position = desiredPos;
            m_target = desiredTarget;
        }
    }
}

inline void Camera::UpdateCinematicMode(double dt) {
    // Cinematic camera logic
}

inline void Camera::UpdateOrbitMode(double dt) {
    // Orbit camera logic
}

inline void Camera::UpdateFreeMode(double dt) {
    // Free camera logic
}

} // namespace render
} // namespace F1Game


