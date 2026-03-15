#include "Track.hpp"
#include "DRS.hpp"
#include <cmath>
#include <raymath.h>

namespace F1Game {

void Track::Load(const TrackData& data) {
    m_data = data;
    m_drsZones = data.drsZones;
    CalculateDistances();
    GenerateSpline();
}

void Track::CalculateDistances() {
    m_totalLength = 0.0f;
    for (size_t i = 0; i < m_data.waypoints.size(); i++) {
        if (i > 0) {
            Vector3 diff = {
                m_data.waypoints[i].position.x - m_data.waypoints[i-1].position.x,
                0,
                m_data.waypoints[i].position.z - m_data.waypoints[i-1].position.z
            };
            m_data.waypoints[i].length = sqrtf(diff.x*diff.x + diff.z*diff.z);
            m_totalLength += m_data.waypoints[i].length;
        }
    }
}

void Track::GenerateSpline() {
    m_splinePoints.clear();
    if (m_data.waypoints.size() < 4) return;

    // Catmull-Rom spline: interpolate to ~800 points (dense for AI)
    const int TARGET_POINTS = 800;
    for (int i = 0; i <= TARGET_POINTS; ++i) {
        float t = (float)i / TARGET_POINTS;  // 0 to 1 lap
        TrackWaypoint wp;
        wp.position = GetSplinePointRaw(t);
        // Direction/curvature approx from neighbors
        float t_next = t + 1.0f / TARGET_POINTS;
        Vector3 dir = Vector3Subtract(GetSplinePointRaw(t_next), wp.position);
        dir.y = 0;
        wp.direction = Vector3Normalize(dir);
        wp.length = Vector3Length(dir) * TARGET_POINTS;
        wp.curvature = 0.0f;  // Compute from 2nd deriv if needed
        wp.section = TrackSection::Corner;
        wp.racingLineOffset = 0.0f;
        m_splinePoints.push_back(wp);
    }
}

Vector3 Track::GetSplinePointRaw(float t) const {
    // Catmull-Rom closed loop
    size_t n = m_data.waypoints.size();
    if (n < 4) return m_data.waypoints[0].position;

    float u = fmodf(t, 1.0f) * n;
    int i = (int)u;
    float f = u - i;

    // Handle loop
    int i0 = (i + n - 1) % n;
    int i1 = i % n;
    int i2 = (i + 1) % n;
    int i3 = (i + 2) % n;

    const auto& p0 = m_data.waypoints[i0].position;
    const auto& p1 = m_data.waypoints[i1].position;
    const auto& p2 = m_data.waypoints[i2].position;
    const auto& p3 = m_data.waypoints[i3].position;

    // Catmull-Rom
    Vector3 a = Vector3Scale(p0, -0.5f);
    Vector3 b = Vector3Scale(p1, 1.5f);
    Vector3 c = Vector3Scale(p2, -1.5f);
    Vector3 d = Vector3Scale(p3, 0.5f);
    Vector3 pos = Vector3Add(Vector3Add(Vector3Add(a, b), c), d);
    pos = Vector3Scale(pos, f * f * f);
    pos = Vector3Add(pos, Vector3Scale(Vector3Subtract(Vector3Scale(p1, 2.0f), p0), f * f));
    pos = Vector3Add(pos, Vector3Scale(Vector3Add(p2, Vector3Scale(p0, -1.0f)), f));
    pos.y = 0.0f;
    return pos;
}

Vector3 Track::GetSplinePoint(float progress) const {
    if (m_splinePoints.empty()) return m_data.waypoints[0].position;
    int idx = (int)(progress * (m_splinePoints.size() - 1));
    return m_splinePoints[idx].position;
}

// Rest unchanged...
int Track::GetClosestWaypoint(const Vector3& position) const {
    float minDist = 1e10f;
    int closest = 0;

    for (size_t i = 0; i < m_data.waypoints.size(); i++) {
        Vector3 diff = {
            position.x - m_data.waypoints[i].position.x,
            0,
            position.z - m_data.waypoints[i].position.z
        };
        float dist = Vector3Length(diff);
        if (dist < minDist) {
            minDist = dist;
            closest = i;
        }
    }
    return closest;
}

bool Track::IsOnTrack(const Vector3& position, float width) const {
    int closest = GetClosestWaypoint(position);
    const auto& wp = m_data.waypoints[closest];

    Vector3 toPoint = {
        position.x - wp.position.x,
        0,
        position.z - wp.position.z
    };
    float dist = Vector3Length(toPoint);

    return dist < width * 0.5f;
}

float Track::GetTrackWidth(int waypointIndex) const {
    if (waypointIndex >= 0 && waypointIndex < (int)m_data.waypoints.size()) {
        const auto& section = m_data.waypoints[waypointIndex].section;
        switch (section) {
            case TrackSection::Straight: return 12.0f;
            case TrackSection::Hairpin: return 10.0f;
            default: return 11.0f;
        }
    }
    return 11.0f;
}

void Track::Draw() const {
    // Draw waypoints
    for (size_t i = 0; i < m_data.waypoints.size(); i++) {
        const auto& wp = m_data.waypoints[i];
        Vector3 next = m_data.waypoints[(i + 1) % m_data.waypoints.size()].position;
        DrawLine3D({wp.position.x, 0.1f, wp.position.z}, {next.x, 0.1f, next.z}, GREEN);
        DrawSphere(wp.position, 0.3f, RED);
    }

    // Draw DRS zones (blue boxes)
    for (const auto& zone : m_drsZones) {
        if (zone.enabled) {
            DrawCube({(float)(zone.startWaypoint * 10), 0, 0}, 10, 1, 100, BLUE);
        }
    }
}

// Monaco with DRS zones
TrackData CreateMonacoTrack() {
    TrackData data;
    data.name = "Circuit de Monaco";
    data.country = "Monaco";
    data.length = 3337;
    data.corners = 19;
    data.type = TrackType::Street;

    data.waypoints = {
        {{0, 0, 0}, {1, 0, 0}, 0, 0, TrackSection::StartFinish, 0},
        {{50, 0, 0}, {1, 0, 0}, 0, 50, TrackSection::Straight, 0},
        {{100, 0, 0}, {0.7f, 0, 0.7f}, 1.0f, 30, TrackSection::Hairpin, 2},
        {{115, 0, 35}, {0, 0, 1}, 0.5f, 40, TrackSection::Corner, 3},
        {{115, 0, 75}, {0, 0, 1}, 0, 40, TrackSection::Straight, 0},
        {{100, 0, 115}, {-0.7f, 0, 0.7f}, -0.5f, 50, TrackSection::Corner, 0},
        {{50, 0, 130}, {-1, 0, 0}, 0, 50, TrackSection::Straight, 0},
        {{0, 0, 130}, {-1, 0, 0}, 0, 50, TrackSection::Straight, 0},
        {{-50, 0, 130}, {-1, 0, 0}, 0.3f, 50, TrackSection::Corner, -1},
        {{-90, 0, 110}, {-0.7f, 0, -0.7f}, 0.8f, 40, TrackSection::Corner, -2},
        {{-100, 0, 75}, {0, 0, -1}, 0.5f, 35, TrackSection::Corner, 0},
        {{-100, 0, 35}, {0, 0, -1}, 0, 40, TrackSection::Straight, 0},
        {{-90, 0, 0}, {0.7f, 0, -0.7f}, -0.5f, 30, TrackSection::Corner, 1},
        {{-50, 0, -15}, {1, 0, 0}, 0, 50, TrackSection::Straight, 0},
        {{0, 0, -15}, {1, 0, 0}, 0, 15, TrackSection::StartFinish, 0},
    };

    // Monaco DRS zones (2026 style: 2 zones)
    data.drsZones = {
        {0, 5, 1.0f, true},   // Start straight to tunnel exit
        {7, 12, 1.0f, true}   // Post-harbor to Rascasse
    };

    return data;
}

// Placeholder for others
TrackData CreateSilverstoneTrack() {
    TrackData data;
    data.name = "Silverstone Circuit";
    data.country = "UK";
    data.length = 5891;
    data.corners = 18;
    data.type = TrackType::GrandPrix;
    // ... waypoints from previous
    data.drsZones = {{0,3},{10,13}};
    return data;
}

TrackData CreateSuzukaTrack() {
    TrackData data;
    data.name = "Suzuka Circuit";
    data.country = "Japan";
    data.length = 5807;
    data.corners = 18;
    data.type = TrackType::GrandPrix;
    // ... 
    data.drsZones = {{0,2},{8,11}};
    return data;
}

} // namespace F1Game

