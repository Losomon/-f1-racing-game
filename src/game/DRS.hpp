#pragma once
#include <raylib.h>
#include "../physics/VehicleBody.hpp"

namespace F1Game {
namespace Game {

struct DRSZone {
    int startWaypoint;
    int endWaypoint;
    float activationGap = 1.0f; // seconds
    bool enabled = true;
};

class DRS {
public:
    DRS();
    ~DRS() = default;

    void Update(float delta, float gapToCarAhead, bool inZone, bool driverPressed);
    bool IsOpen() const { return m_isOpen; }
    float GetTimer() const { return m_timer; }
    void SetDetectionZone(int zoneID) { m_detectionZoneID = zoneID; }

private:
    bool m_isOpen = false;
    float m_timer = 0.0f;
    int m_detectionZoneID = -1;
};

inline DRS::DRS() = default;

inline void DRS::Update(float delta, float gapToCarAhead, bool inZone, bool driverPressed) {
    constexpr float activationGap = 1.0f; // seconds (stub)
    if (inZone && gapToCarAhead <= activationGap && !m_isOpen) {
        m_isOpen = true;
        m_timer = 0.0f;
    }
    if (m_isOpen) {
        m_timer += delta;
        if (m_timer > 0.8f || !inZone) {
            m_isOpen = false;
        }
    }
    // Driver override in quali (not race)
    if (driverPressed && inZone) m_isOpen = true;
}

} // Game
} // F1Game

