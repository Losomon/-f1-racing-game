#include "Car.hpp"

namespace F1Game {

Car::Car() {
}

void Car::Setup(const Driver& driver, const CarConfig& config) {
    m_driver = driver;
    m_config = config;
    
    // Reset state
    m_currentLapTime = 0.0f;
    m_bestLap = 1e10f;
    m_currentLap = 0;
    m_position = 0;
    m_inPit = false;
    m_pitTime = 0.0f;
    m_tireAge = 0;
    m_fuelLoad = 110.0f;
    m_damage = 0.0f;
    m_finished = false;
}

void Car::Update(double dt) {
    if (m_finished) return;
    
    // Update physics
    if (!m_inPit) {
        m_physics.Update((float)dt);
    }
    
    // Update lap time
    m_currentLapTime += (float)dt;
    
    // Fuel consumption (simplified ~0.1kg per lap per second at full throttle)
    float fuelRate = 0.1f * m_input.throttle * (float)dt;
    m_fuelLoad -= fuelRate;
    if (m_fuelLoad < 0) m_fuelLoad = 0;
}

void Car::SetInput(const Physics::VehicleInput& input) {
    m_input = input;
}

void Car::AddLapTime(float time) {
    if (time < m_bestLap) {
        m_bestLap = time;
    }
    m_currentLapTime = 0.0f;
    m_currentLap++;
}

void Car::EnterPit() {
    m_inPit = true;
    m_pitTime = 0.0f;
}

void Car::ExitPit() {
    m_inPit = false;
    // Typical pit stop ~2 seconds
}

void Car::AddDamage(float amount) {
    m_damage += amount;
    if (m_damage >= 100.0f) {
        m_damage = 100.0f;
        m_finished = true;
    }
}

} // namespace F1Game



