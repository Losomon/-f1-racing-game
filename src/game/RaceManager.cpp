#include "RaceManager.hpp"
#include <algorithm>
#include <iostream>

namespace F1Game {

RaceManager::RaceManager() {
}

void RaceManager::SetupRace(RaceType type, const Track& track, int laps) {
    m_type = type;
    m_track = track;
    m_totalLaps = laps;
    m_currentLap = 0;
    m_raceTime = 0.0f;
    m_state = RaceState::PreRace;

    // Set pit window based on race length
    m_pitWindowStart = (int)(laps * 0.3f);
    m_pitWindowEnd = (int)(laps * 0.7f);
}

void RaceManager::AddCar(Car* car) {
    m_cars.push_back(car);
    car->SetPosition((int)m_cars.size());
}

void RaceManager::StartRace() {
    m_state = RaceState::Racing;
    m_currentLap = 1;
    
    std::cout << "=== Race Start! ===" << std::endl;
    std::cout << "Track: " << m_track.GetName() << std::endl;
    std::cout << "Laps: " << m_totalLaps << std::endl;
    std::cout << "Cars: " << m_cars.size() << std::endl;
}

void RaceManager::Update(double dt) {
    if (m_state != RaceState::Racing && m_state != RaceState::PitWindow) {
        return;
    }

    m_raceTime += (float)dt;

    // Update all cars
    for (auto* car : m_cars) {
        car->Update(dt);
    }

    // Check lap completion
    CheckLapCompletion();

    // Update positions
    UpdatePositions();

    // Calculate gaps
    CalculateGaps();

    // Check for race finish
    if (IsRaceFinished()) {
        m_state = RaceState::Finished;
        std::cout << "=== Race Finished! ===" << std::endl;
    }
}

void RaceManager::UpdatePositions() {
    // Sort cars by distance along track (simplified)
    // In real implementation, would use actual track position
    std::sort(m_cars.begin(), m_cars.end(), [](Car* a, Car* b) {
        const auto& stateA = a->GetPhysics().GetState();
        const auto& stateB = b->GetPhysics().GetState();
        
        // Compare by lap first, then by distance along track
        if (a->GetCurrentLap() != b->GetCurrentLap()) {
            return a->GetCurrentLap() > b->GetCurrentLap();
        }
        
        // Simple distance comparison (would use track position in real impl)
        float distA = stateA.position.x + stateA.position.z;
        float distB = stateB.position.x + stateB.position.z;
        return distA > distB;
    });

    // Update positions
    for (size_t i = 0; i < m_cars.size(); i++) {
        m_cars[i]->SetPosition((int)i + 1);
    }
}

void RaceManager::CheckLapCompletion() {
    for (auto* car : m_cars) {
        const auto& state = car->GetPhysics().GetState();
        
        // Simple lap detection: crossed start line (x=0)
        // In real implementation, would check track waypoints
        if (state.position.x > 0 && state.position.x < 10 && state.position.z < 5) {
            // Check if this is a new lap (simple debounce)
            if (car->GetCurrentLapTime() > 30.0f) {  // Minimum lap time
                car->AddLapTime(car->GetCurrentLapTime());
                
                if (car->GetCurrentLap() > m_currentLap) {
                    m_currentLap = car->GetCurrentLap();
                }
            }
        }
    }
}

void RaceManager::CalculateGaps() {
    if (m_cars.empty()) return;

    // Leader's time is reference
    Car* leader = m_cars[0];
    float leaderTime = leader->GetCurrentLapTime() + (leader->GetCurrentLap() - 1) * 90.0f;

    for (size_t i = 0; i < m_cars.size(); i++) {
        // Gaps calculated in update
    }
}

bool RaceManager::IsRaceFinished() const {
    if (m_cars.empty()) return true;
    
    // Check if leader completed all laps
    return m_cars[0]->GetCurrentLap() >= m_totalLaps;
}

std::vector<LeaderboardEntry> RaceManager::GetLeaderboard() const {
    std::vector<LeaderboardEntry> entries;
    
    for (size_t i = 0; i < m_cars.size(); i++) {
        LeaderboardEntry entry;
        entry.position = i + 1;
        entry.car = m_cars[i];
        entry.lap = m_cars[i]->GetCurrentLap();
        
        if (i == 0) {
            entry.gap = 0.0f;
            entry.interval = 0.0f;
        } else {
            // Simplified gap calculation
            entry.gap = (float)(i * 2.0);  // 2 seconds per position
            entry.interval = 2.0f;
        }
        
        entries.push_back(entry);
    }
    
    return entries;
}

std::vector<RaceResult> RaceManager::GetResults() const {
    std::vector<RaceResult> results;
    
    for (auto* car : m_cars) {
        RaceResult result;
        result.position = car->GetPosition();
        result.driverName = car->GetDriver().name;
        result.team = car->GetDriver().name;  // Would use car config
        result.time = m_raceTime;
        result.laps = car->GetCurrentLap();
        result.bestLap = car->GetBestLap();
        result.finished = !car->IsFinished();
        
        results.push_back(result);
    }
    
    return results;
}

void RaceManager::DrawHUD() const {
    // Draw position
    if (!m_cars.empty()) {
        DrawText(TextFormat("POS: %d/%d", m_cars[0]->GetPosition(), (int)m_cars.size()), 
                 10, 10, 20, GREEN);
        
        // Draw lap
        DrawText(TextFormat("LAP: %d/%d", m_currentLap, m_totalLaps), 
                 10, 35, 20, GREEN);
        
        // Draw time
        int minutes = (int)m_raceTime / 60;
        int seconds = (int)m_raceTime % 60;
        DrawText(TextFormat("TIME: %d:%02d", minutes, seconds), 
                 10, 60, 20, GREEN);
        
        // Draw speed
        float speed = m_cars[0]->GetPhysics().GetState().speedKmh;
        DrawText(TextFormat("SPEED: %.0f km/h", speed), 
                 10, 85, 20, YELLOW);
    }
}

void RaceManager::TriggerSafetyCar() {
    m_safetyCar = true;
    m_virtualSafetyCar = false;
    m_state = RaceState::Racing;
    std::cout << "SAFETY CAR DEPLOYED" << std::endl;
}

void RaceManager::TriggerVirtualSafetyCar() {
    m_virtualSafetyCar = true;
    std::cout << "VIRTUAL SAFETY CAR" << std::endl;
}

void RaceManager::ClearSafetyCar() {
    m_safetyCar = false;
    m_virtualSafetyCar = false;
    std::cout << "TRACK CLEAR" << std::endl;
}

} // namespace F1Game



