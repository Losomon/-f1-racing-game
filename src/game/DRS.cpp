#include "DRS.hpp"

namespace F1Game {
namespace Game {

DRS::DRS() {
    m_isOpen = false;
    m_timer = 0.0f;
    m_detectionZoneID = -1;
}

void DRS::Update(float delta, float gapToCarAhead, bool inZone, bool driverPressed) {
    if (inZone && gapToCarAhead <= 1.0f && !m_isOpen) {
        m_isOpen = true;
        m_timer = 0.0f;
    }
    if (m_isOpen) {
        m_timer += delta;
        if (m_timer > 0.8f || !inZone) {
            m_isOpen = false;
            m_timer = 0.0f;
        }
    }
    // Manual override for quali/testing
    if (driverPressed && inZone) {
        m_isOpen = true;
        m_timer = 0.0f;
    }
}

} // Game
} // F1Game

