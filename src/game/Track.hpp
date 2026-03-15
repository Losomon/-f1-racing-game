#pragma once

#include <raylib.h>
#include <vector>
#include <string>

namespace F1Game {

// Track types
enum class TrackType {
    GrandPrix,
    Street,
    Historic
};

// Track section types
enum class TrackSection {
    StartFinish,
    Turn1,
    Turn2,
    Corner,
    Straight,
    Chicanes,
    Hairpin
};

// Track waypoint for AI and racing line
struct TrackWaypoint {
    Vector3 position;
    Vector3 direction;    // tangent direction
    float curvature;      // 0 = straight, positive = left, negative = right
    float length;         // meters
    TrackSection section;
    float racingLineOffset; // optimal path offset from center
};

struct DRSZone {
    int startWaypoint;
    int endWaypoint;
    float activationGap = 1.0f; // seconds
    bool enabled = true;
};


// Track data
struct TrackData {
    std::string name;
    std::string country;
    int length;           // meters
    int corners;          // number of turns
    float startLat;       // starting latitude
    float startLon;       // starting longitude
    TrackType type;
    std::vector<TrackWaypoint> waypoints;
    std::vector<DRSZone> drsZones;
};


class Track {
public:
    Track() = default;
    ~Track() = default;

    // Load track from data
    void Load(const TrackData& data);

    // Get waypoints for racing line
    const std::vector<TrackWaypoint>& GetWaypoints() const { return m_data.waypoints; }

    // Find closest waypoint to position
    int GetClosestWaypoint(const Vector3& position) const;

    // Get track properties
    const std::string& GetName() const { return m_data.name; }
    int GetLength() const { return m_data.length; }
    int GetCornerCount() const { return m_data.corners; }

    // Check if position is on track (for collision)
    bool IsOnTrack(const Vector3& position, float width = 8.0f) const;

    // Get track width at waypoint
    float GetTrackWidth(int waypointIndex) const;

    // Draw track (simple debug visualization)
    void Draw() const;

    // Phase 4 DRS/Spline
    const std::vector<DRSZone>& GetDRSZones() const { return m_drsZones; }
    bool IsInDRSZone(int waypointIdx) const;
    Vector3 GetSplinePoint(float progress) const; // 0.0-1.0 lap position
    void GenerateSpline();


private:
    TrackData m_data;
    std::vector<DRSZone> m_drsZones;
    std::vector<TrackWaypoint> m_splinePoints;
    float m_totalLength = 0.0f;

    // Pre-calculate cumulative distances
    void CalculateDistances();
    Vector3 GetSplinePointRaw(float t) const;
};

inline bool Track::IsInDRSZone(int waypointIdx) const {
    for (const auto& zone : m_drsZones) {
        if (waypointIdx >= zone.startWaypoint && waypointIdx <= zone.endWaypoint) {
            return zone.enabled;
        }
    }
    return false;
}


// Sample tracks
TrackData CreateMonacoTrack();
TrackData CreateSilverstoneTrack();
TrackData CreateSuzukaTrack();

} // namespace F1Game



