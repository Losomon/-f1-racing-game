#include "SteamManager.hpp"
#include <stdio.h>

// #include "steam/steam_api.h" // Steamworks SDK 1.59 (lib/steam/ required)

namespace F1Game {

SteamManager::SteamManager() : m_initialized(false) {}

SteamManager::~SteamManager() {}

void SteamManager::Init() {
    printf("[SteamManager] Phase5 stub initialized (SDK comment out)\n");
    // SteamAPI_Init();
    // m_lapLeaderboard = SteamUserStats()->FindOrCreateLeaderboard("FastestLap_Monaco", k_ELeaderboardSortMethodDescending, k_ELeaderboardDisplayTypeTimeSeconds);
    m_initialized = true;
}

void SteamManager::UploadLapTime(float lapTimeSeconds, bool isPersonalBest) {
    if (isPersonalBest) {
        printf("[Steam] Uploaded Monaco lap: %.3f seconds\n", lapTimeSeconds);
        // SteamUserStats()->UploadLeaderboardScore(m_lapLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)(lapTimeSeconds * 1000));
    }
}

void SteamManager::UnlockAchievement(const std::string& id) {
    printf("[Steam] Unlocked achievement: %s\n", id.c_str());
    // SteamUserStats()->SetAchievement(id.c_str()); SteamUserStats()->StoreStats();
}

} // namespace F1Game

