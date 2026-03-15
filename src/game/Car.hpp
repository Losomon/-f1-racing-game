#pragma once

#include <raylib.h>
#include <string>
#include "../physics/VehicleBody.hpp"

namespace F1Game {

// Driver info
struct Driver {
    std::string name;
    int number;
    float skill;      // 0-1 (overall skill)
    float aggression; // 0-1
    float consistency; // 0-1
    float pace;       // 0-1
};

// Car configuration
struct CarConfig {
    std::string team;
    std::string chassis;
    int year;
    float enginePower;    // kW
    float reliability;   // 0-1
    float topSpeed;      // km/h
};

class Car {
public:
    Car();
    ~Car() = default;

    // Initialize with driver and config
    void Setup(const Driver& driver, const CarConfig& config);

    // Update car state
    void Update(double dt);

    // Get physics body
    Physics::VehicleBody& GetPhysics() { return m_physics; }
    const Physics::VehicleBody& GetPhysics() const { return m_physics; }

    // Get driver info
    const Driver& GetDriver() const { return m_driver; }
    Driver& GetDriver() { return m_driver; }

    // Lap timing
    void AddLapTime(float time);
    float GetBestLap() const { return m_bestLap; }
    float GetCurrentLapTime() const { return m_currentLapTime; }
    int GetCurrentLap() const { return m_currentLap; }

    // Race position
    int GetPosition() const { return m_position; }
    void SetPosition(int pos) { m_position = pos; }

    // Pit stop
    void EnterPit();
    void ExitPit();
    bool IsInPit() const { return m_inPit; }
    float GetPitTime() const { return m_pitTime; }

    // Tire and fuel
    void SetTireAge(int laps) { m_tireAge = laps; }
    int GetTireAge() const { return m_tireAge; }
    void SetFuelLoad(float kg) { m_fuelLoad = kg; }
    float GetFuelLoad() const { return m_fuelLoad; }

    // Damage
    void AddDamage(float amount);
    float GetDamage() const { return m_damage; }
    bool IsFinished() const { return m_finished; }

    // Input
    void SetInput(const Physics::VehicleInput& input);
    const Physics::VehicleInput& GetInput() const { return m_input; }

private:
    Driver m_driver;
    CarConfig m_config;
    Physics::VehicleBody m_physics;
    Physics::VehicleInput m_input;

    // Timing
    float m_currentLapTime = 0.0f;
    float m_bestLap = 1e10f;
    int m_currentLap = 0;
    int m_position = 0;

    // Pit
    bool m_inPit = false;
    float m_pitTime = 0.0f;

    // Status
    int m_tireAge = 0;
    float m_fuelLoad = 110.0f; // kg (typical F1 capacity)
    float m_damage = 0.0f;
    bool m_finished = false;
};

} // namespace F1Game



