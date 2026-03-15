#pragma once

#include <raylib.h>
#include <string>
#include <cstdint>

namespace F1Game {

// Stub Steam types so the real SDK is not required.
using SteamLeaderboard_t = std::int64_t;

class SteamManager {
public:
    SteamManager();
    ~SteamManager();

    void Init();
    void UploadLapTime(float lapTimeSeconds, bool isPersonalBest);
    void UnlockAchievement(const std::string& id);
    bool IsSteamRunning() const { return true; } // stub

private:
    bool m_initialized;
    SteamLeaderboard_t m_lapLeaderboard; // stub
};

} // namespace F1Game

