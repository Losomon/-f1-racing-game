#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <chrono>
#include "ECS.hpp"
#include "EventBus.hpp"
#include "../render/Renderer.hpp"
#include "../game/Track.hpp"
#include "../game/Car.hpp"
#include "../game/RaceManager.hpp"


namespace F1Game {



// Fixed timestep configuration
constexpr double FIXED_DT = 1.0 / 120.0;  // 120Hz physics
constexpr int MAX_SUBSTEPS = 5;

class Engine {
public:
    Engine();
    ~Engine();


    // Core systems
    std::unique_ptr<ECS> ecs;
    std::unique_ptr<EventBus> eventBus;

    // Phase5 demo systems
    Render::Renderer* m_renderer = nullptr;

    RaceManager* m_raceManager = nullptr;
    Track* m_track = nullptr;
    Car* m_playerCar = nullptr;

    void SetRenderer(Render::Renderer* r) { m_renderer = r; }
    void SetRaceManager(RaceManager* r) { m_raceManager = r; }
    void SetTrack(Track* t) { m_track = t; }
    void SetPlayerCar(Car* c) { m_playerCar = c; }


    // Initialization
    void Init(int screenWidth, int screenHeight, const char* title);
    void Shutdown();

    // Main game loop
    void Run();

    // Update functions (called at fixed timestep)
    void UpdateFixed(double dt);

    // Render functions (called every frame)
    void Render(double alpha);

    // Window state
    bool ShouldClose() const;
    double GetInterpolationAlpha() const { return m_accumulator / FIXED_DT; }

private:
    double m_accumulator = 0.0;
    double m_prevTime = 0.0;
    double m_currentTime = 0.0;
    bool m_running = false;

    // Internal update
    void ProcessEvents();
};

// Global engine instance
extern Engine* g_engine;

} // namespace F1Game


