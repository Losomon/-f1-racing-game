#pragma once

#include <raylib.h>
#include <vector>
#include <string>
#include "Car.hpp"
#include "Track.hpp"

namespace F1Game {

// Race types
enum class RaceType {
    TimeTrial,
    Practice,
    Qualifying,
    Sprint,
    GrandPrix
};

// Race session state
enum class RaceState {
    PreRace,
    FormationLap,
    Racing,
    PitWindow,
    CheckeredFlag,
    Finished
};

// Race result
struct RaceResult {
    int position;
    std::string driverName;
    std::string team;
    float time;           // total time or gap
    int laps;
    float bestLap;
    bool finished;
};

// Leaderboard entry
struct LeaderboardEntry {
    int position;
    Car* car;
    float gap;           // gap to leader
    float interval;      // interval to car ahead
    int lap;
};

class RaceManager {
public:
    RaceManager();
    ~RaceManager() = default;

    // Setup race
    void SetupRace(RaceType type, const Track& track, int laps);

    // Add car to race
    void AddCar(Car* car);

    // Start race
    void StartRace();

    // Update race (called every frame)
    void Update(double dt);

    // Race control
    void TriggerSafetyCar();
    void TriggerVirtualSafetyCar();
    void ClearSafetyCar();

    // Getters
    RaceState GetState() const { return m_state; }
    RaceType GetType() const { return m_type; }
    const Track& GetTrack() const { return m_track; }
    int GetTotalLaps() const { return m_totalLaps; }
    int GetCurrentLap() const { return m_currentLap; }
    float GetRaceTime() const { return m_raceTime; }

    // Get leaderboard
    std::vector<LeaderboardEntry> GetLeaderboard() const;

    // Check for race finish
    bool IsRaceFinished() const;

    // Get results
    std::vector<RaceResult> GetResults() const;

    // Draw HUD
    void DrawHUD() const;

private:
    RaceType m_type = RaceType::GrandPrix;
    RaceState m_state = RaceState::PreRace;
    Track m_track;
    int m_totalLaps = 50;
    int m_currentLap = 0;
    float m_raceTime = 0.0f;

    std::vector<Car*> m_cars;

    // Safety car
    bool m_safetyCar = false;
    bool m_virtualSafetyCar = false;
    float m_safetyCarTime = 0.0f;

    // Pit window
    int m_pitWindowStart = 18;
    int m_pitWindowEnd = 35;

    // Update positions
    void UpdatePositions();

    // Check lap completion
    void CheckLapCompletion();

    // Calculate gaps
    void CalculateGaps();
};

} // namespace F1Game



