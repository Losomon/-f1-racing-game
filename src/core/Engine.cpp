#include "Engine.hpp"

namespace F1Game {

Engine* g_engine = nullptr;

Engine::Engine()
    : ecs(std::make_unique<ECS>())
    , eventBus(std::make_unique<EventBus>()) {
}

Engine::~Engine() {
    Shutdown();
}

void Engine::Init(int screenWidth, int screenHeight, const char* title) {
    // Initialize Raylib with Phase5 perf
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, title);

    ChangeDirectory("..");  // Fix working directory for assets (out/ -> f1-game/)

    SetTargetFPS(60);

    m_prevTime = GetTime();
    m_running = true;
    m_accumulator = 0.0;

    g_engine = this;
}


void Engine::Shutdown() {
    if (m_running) {
        CloseWindow();
        m_running = false;
    }
}

void Engine::Run() {
    while (!ShouldClose() && m_running) {
        m_currentTime = GetTime();
        double frameTime = m_currentTime - m_prevTime;
        m_prevTime = m_currentTime;

        // Clamp frame time to prevent spiral of death
        if (frameTime > 0.25) {
            frameTime = 0.25;
        }

        m_accumulator += frameTime;

        // Fixed timestep updates
        int substepCount = 0;
        while (m_accumulator >= FIXED_DT && substepCount < MAX_SUBSTEPS) {
            UpdateFixed(FIXED_DT);
            m_accumulator -= FIXED_DT;
            substepCount++;
        }

        // Interpolation alpha for smooth rendering
        double alpha = m_accumulator / FIXED_DT;

        // Render
        BeginDrawing();
        ClearBackground(BLACK);
        Render(alpha);
        EndDrawing();
    }
}


void Engine::UpdateFixed(double dt) {
    // Process any pending events
    ProcessEvents();

    // Phase5 demo update
    if (m_raceManager) m_raceManager->Update(dt);

    // Update physics systems here
    // Example: physicsSystem.Update(dt);
    
    // Update game logic systems
    // Example: gameSystem.Update(dt);

}



void Engine::Render(double alpha) {
    // Phase5 full render with track + HUD
    if (m_renderer) {
        m_renderer->UpdateCamera((float)alpha);
        m_renderer->RenderScene();
    }
    if (m_raceManager) {
        m_raceManager->DrawHUD();
    }
    
    // Draw debug info
    DrawFPS(10, 10);
    DrawText(TextFormat("Alpha: %.3f", alpha), 10, 30, 20, GREEN);

}



void Engine::ProcessEvents() {
    if (!m_playerCar) return;

    Physics::VehicleInput input{0};
    
    // Phase5 Controller + keyboard
    if (IsGamepadAvailable(0)) {
        float pad_throttle = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);
        float pad_brake = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER);
        input.steering = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        
        const float DEADZONE = 0.08f;
        if (abs(pad_throttle) > DEADZONE) input.throttle = pad_throttle;
        if (abs(pad_brake) > DEADZONE) input.brake = pad_brake;
        
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
            m_playerCar->GetPhysics().SetDRS(!m_playerCar->GetPhysics().IsDRSActive());
        }
        
        // Rumble
        if (m_playerCar->GetPhysics().GetTireWearFL() > 85.0f) {
            SetGamepadVibration(0, 0.4f, 0.2f, 120);
        }
    } else {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) input.throttle = 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) input.brake = 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) input.steering = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input.steering = 1.0f;
        if (IsKeyPressed(KEY_E)) {
            m_playerCar->GetPhysics().SetDRS(!m_playerCar->GetPhysics().IsDRSActive());
        }
    }
    
    m_playerCar->GetPhysics().SetInputs(input.throttle, input.brake, input.steering);
    
    // PollEvents is handled by raylib internally
}


bool Engine::ShouldClose() const {
    return WindowShouldClose();
}

} // namespace F1Game
