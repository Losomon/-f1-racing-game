#include "Renderer.hpp"
#include "../game/Car.hpp"
#include "../game/Track.hpp"
#include "ShaderManager.hpp"
#include <memory>
#include <rlgl.h>
#include <raymath.h>
#include <cmath>
#include <cstdlib>

namespace F1Game {
namespace Render {

Renderer::Renderer() {
    m_camera = {0};
    m_camera.position = {0, 5, -10};
    m_camera.target = {0, 0, 0};
    m_camera.up = {0, 1, 0};
    m_camera.fovy = 60.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
    m_shaderManager = std::make_unique<F1Game::render::ShaderManager>();
}

Renderer::~Renderer() {
    Shutdown();
    UnloadModels();
}

void Renderer::Init(int width, int height) {
    m_width = width;
    m_height = height;
    m_initialized = true;

    m_camera.position = Vector3{0.0f, 20.0f, -5.0f};
    m_camera.target   = Vector3{0.0f,  0.0f, 20.0f};

    m_carModel         = LoadModel("assets/models/car/2020-f1-mercedes-benz-w11/source/2020 F1 Mercedes-Benz W11.glb");
    m_trackModel       = LoadModel("assets/models/track/source/trakmonaco.glb");
}

void Renderer::Shutdown() {
    UnloadRenderTexture(m_sceneHDR);
    UnloadRenderTexture(m_velocityTex);
    m_shaderManager.reset();
    m_initialized = false;
}

void Renderer::InitPostFX() {
    Image img = GenImageColor(m_width, m_height, BLANK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    m_sceneHDR = LoadRenderTexture(tex.width, tex.height);
    SetTextureFilter(m_sceneHDR.texture, TEXTURE_FILTER_BILINEAR);
    UnloadTexture(tex);

    m_velocityTex = LoadRenderTexture(m_width, m_height);

    int w = m_width / 2, h = m_height / 2;
    for (int i = 0; i < 6; ++i) {
        m_bloomPyramid[i] = LoadRenderTexture(w, h);
        w /= 2; h /= 2;
    }
}

void Renderer::ToggleScreenshotMode() {
    m_screenshotMode = !m_screenshotMode;
    m_motionBlurEnabled = !m_screenshotMode;
    TraceLog(LOG_INFO, "Screenshot mode: %s", m_screenshotMode ? "ON" : "OFF");
}

void Renderer::RenderToHDR() {
    BeginTextureMode(m_sceneHDR);
    ClearBackground({20, 20, 30, 255});
    BeginMode3D(m_camera);
    DrawEnvironment();
    EndMode3D();
    EndTextureMode();
}

void Renderer::DoPostProcessing() {
    BeginDrawing();
    DrawTextureRec(m_sceneHDR.texture, {0, 0, (float)m_width, -(float)m_height}, {0, 0}, WHITE);
    DrawText("Phase 3 Rendering - PBR/Post ready! F12 screenshot mode", 10, 10, 20, YELLOW);
    DrawFPS(10, 40);
    EndDrawing();
}

void Renderer::RenderScene() {
    UpdateFollowCamera();
    BeginMode3D(m_camera);
    DrawEnvironment();
    if (m_track) m_track->Draw();
    if (m_targetCar) {
        DrawCar(*m_targetCar);
    }
    EndMode3D();
}

void Renderer::BeginFrame() {}
void Renderer::EndFrame() {}
void Renderer::RenderUI() {
    const char* modeName = "Follow";
    switch (m_cameraMode) {
        case CameraMode::Follow:    modeName = "1: Chase Cam";    break;
        case CameraMode::TV:        modeName = "2: TV Side Cam";  break;
        case CameraMode::Drone:     modeName = "3: Drone Top";    break;
        case CameraMode::Cinematic: modeName = "4: Cinematic";    break;
        case CameraMode::Orbit:     modeName = "5: Cockpit";      break;
        default: break;
    }
    DrawText(TextFormat("CAM: %s", modeName), 10, 150, 18, YELLOW);
    DrawText("Keys 1-5: Switch Camera", 10, 172, 16, GRAY);
}

void Renderer::DrawCar(const F1Game::Car& car) {
    const auto& state = car.GetPhysics().GetState();
    Vector3 pos = state.position;
    float yawRad = state.yaw;

    if (m_carModel.meshCount > 0) {
        Vector3 carDrawPos = {pos.x, pos.y, pos.z};
        Vector3 scale = {0.02f, 0.02f, 0.02f};
        DrawModelEx(m_carModel, carDrawPos, {0.0f, 1.0f, 0.0f}, yawRad * (180.0f / PI) + 180.0f, scale, WHITE);
    } else {
        Vector3 forward = Vector3{sinf(yawRad), 0.0f, cosf(yawRad)};
        Vector3 right   = Vector3{cosf(yawRad), 0.0f, -sinf(yawRad)};

        Vector3 chassisPos = {pos.x, pos.y + 0.4f, pos.z};
        DrawCube(chassisPos, 1.8f, 0.5f, 4.2f, (Color){200, 20, 20, 255});
        Vector3 rearWing = Vector3Add(chassisPos, Vector3Scale(forward, -2.0f));
        rearWing.y += 0.3f;
        DrawCube(rearWing, 2.2f, 0.4f, 0.3f, DARKGRAY);
        Vector3 frontWing = Vector3Add(chassisPos, Vector3Scale(forward, 2.0f));
        frontWing.y -= 0.1f;
        DrawCube(frontWing, 2.0f, 0.15f, 0.4f, DARKGRAY);
        DrawCube(Vector3Add(chassisPos, Vector3Scale(right,  0.9f)), 0.5f, 0.4f, 2.2f, (Color){160, 10, 10, 255});
        DrawCube(Vector3Add(chassisPos, Vector3Scale(right, -0.9f)), 0.5f, 0.4f, 2.2f, (Color){160, 10, 10, 255});
        for (int i = 0; i < 4; i++) {
            float lr = (i % 2 == 0) ? 1.1f : -1.1f;
            float fb = (i < 2) ? 1.3f : -1.3f;
            Vector3 wheelPos = {
                pos.x + lr * right.x + fb * forward.x,
                pos.y + 0.3f,
                pos.z + lr * right.z + fb * forward.z
            };
            DrawCylinder(wheelPos, 0.4f, 0.4f, 0.5f, 16, (Color){30, 30, 30, 255});
        }
    }
}

void Renderer::DrawTrack(const F1Game::Track& track) {
    Vector3 trackScale = {0.005f, 0.005f, 0.005f};
    DrawModelEx(m_trackModel, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0.0f, trackScale, WHITE);

    Vector3 gsScale = {0.002f, 0.002f, 0.002f};
    DrawModelEx(m_grandstandsModel, {-80.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},   0.0f, gsScale, WHITE);
    DrawModelEx(m_grandstandsModel, { 80.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 180.0f, gsScale, WHITE);
}

void Renderer::DrawEnvironment() {
    DrawPlane({0.0f, 0.0f, 0.0f}, {500.0f, 500.0f}, (Color){34, 85, 34, 255});
    DrawPlane({0.0f, 0.005f, 0.0f}, {14.0f, 500.0f}, (Color){55, 55, 55, 255});
    DrawPlane({0.0f, 0.01f,  0.0f}, {0.3f,  500.0f}, WHITE);

    if (m_trackModel.meshCount > 0) {
        Vector3 trackScale = {5.0f, 5.0f, 5.0f};
        DrawModelEx(m_trackModel, {0.0f, 0.0f, 0.0f}, {0,1,0}, 0.0f, trackScale, WHITE);
    }

    if (m_cityModel.meshCount > 0) {
        Vector3 cityScale = {1.0f, 1.0f, 1.0f};
        DrawModelEx(m_cityModel, {0.0f, 0.0f, 0.0f}, {0,1,0}, 0.0f, cityScale, WHITE);
    } else {
        srand(42);
        for (int i = 0; i < 30; i++) {
            float angle  = (float)i * 0.8f;
            float dist   = 150.0f + (float)(rand() % 80);
            float height = (float)(rand() % 25) + 8.0f;
            float width  = (float)(rand() % 6)  + 4.0f;
            Vector3 pos  = {dist * cosf(angle), height * 0.5f, dist * sinf(angle)};
            DrawCube(pos, width, height, width, (Color){180, 160, 130, 255});
        }
    }

    if (m_grandstandsModel.meshCount > 0) {
        Vector3 gsScale = {1.0f, 1.0f, 1.0f};
        DrawModelEx(m_grandstandsModel, {0.0f, 0.0f, 0.0f}, {0,1,0}, 0.0f, gsScale, WHITE);
    } else {
        for (int side = -1; side <= 1; side += 2) {
            for (int i = 0; i < 10; i++) {
                Vector3 pos = {(float)side * 10.0f, 1.5f, (float)i * 20.0f - 90.0f};
                DrawCube(pos, 3.0f, 3.0f, 18.0f, (Color){180, 30, 30, 255});
            }
        }
    }
}

void Renderer::UpdateCamera(float dt) {
    UpdateFollowCamera();
}

void Renderer::UpdateFollowCamera() {
    if (!m_targetCar) return;
    const auto& state = m_targetCar->GetPhysics().GetState();
    Vector3 carPos = state.position;
    float yaw = state.yaw;

    if (IsKeyPressed(KEY_ONE))   m_cameraMode = CameraMode::Follow;
    if (IsKeyPressed(KEY_TWO))   m_cameraMode = CameraMode::TV;
    if (IsKeyPressed(KEY_THREE)) m_cameraMode = CameraMode::Drone;
    if (IsKeyPressed(KEY_FOUR))  m_cameraMode = CameraMode::Cinematic;
    if (IsKeyPressed(KEY_FIVE))  m_cameraMode = CameraMode::Orbit;

    switch (m_cameraMode) {

        case CameraMode::Follow: {
            float camX = carPos.x - 10.0f * sinf(yaw);
            float camZ = carPos.z - 10.0f * cosf(yaw);
            float camY = carPos.y + 4.0f;
            m_camera.position = Vector3{camX, camY, camZ};
            m_camera.target   = Vector3{carPos.x + 3.0f * sinf(yaw),
                                  carPos.y + 1.0f,
                                  carPos.z + 3.0f * cosf(yaw)};
            break;
        }

        case CameraMode::TV: {
            float camX = carPos.x + 20.0f * cosf(yaw);
            float camZ = carPos.z - 20.0f * sinf(yaw);
            float camY = carPos.y + 3.0f;
            m_camera.position = Vector3{camX, camY, camZ};
            m_camera.target   = Vector3{carPos.x, carPos.y + 0.5f, carPos.z};
            break;
        }

        case CameraMode::Drone: {
            m_camera.position = Vector3{carPos.x, carPos.y + 30.0f, carPos.z};
            m_camera.target   = Vector3{carPos.x, carPos.y, carPos.z};
            m_camera.up       = Vector3{sinf(yaw), 0.0f, cosf(yaw)};
            break;
        }

        case CameraMode::Cinematic: {
            float camX = carPos.x + 12.0f * sinf(yaw);
            float camZ = carPos.z + 12.0f * cosf(yaw);
            float camY = carPos.y + 1.5f;
            m_camera.position = Vector3{camX, camY, camZ};
            m_camera.target   = Vector3{carPos.x, carPos.y + 0.8f, carPos.z};
            break;
        }

        case CameraMode::Orbit: {
            float camX = carPos.x + 0.0f;
            float camZ = carPos.z + 0.0f;
            float camY = carPos.y + 0.8f;
            m_camera.position = Vector3{camX, camY, camZ};
            m_camera.target   = Vector3{carPos.x + 20.0f * sinf(yaw),
                                  carPos.y + 0.8f,
                                  carPos.z + 20.0f * cosf(yaw)};
            break;
        }

        default: break;
    }

    if (m_cameraMode != CameraMode::Drone) {
        m_camera.up = Vector3{0.0f, 1.0f, 0.0f};
    }
}

void Renderer::SetCameraMode(CameraMode mode) {
    m_cameraMode = mode;
}

void Renderer::SetTargetCar(F1Game::Car* car) {
    m_targetCar = car;
}

void Renderer::SetTrack(F1Game::Track* track) {
    m_track = track;
}

void Renderer::UpdateOrbitCamera() {}

void Renderer::DrawDebugGrid() {
    for (int i = -50; i <= 50; i += 10) {
        DrawLine3D(Vector3{(float)i, 0, -50}, Vector3{(float)i, 0,  50}, DARKGRAY);
        DrawLine3D(Vector3{-50, 0, (float)i}, Vector3{ 50, 0, (float)i}, DARKGRAY);
    }
}

void Renderer::DrawSkybox() {}
void Renderer::SetVignette(float intensity) {}
void Renderer::SetColorGrading(const char* lutPath) {}

void Renderer::UnloadModels() {
    if (m_carModel.meshCount > 0)         UnloadModel(m_carModel);
    if (m_trackModel.meshCount > 0)       UnloadModel(m_trackModel);
    if (m_grandstandsModel.meshCount > 0) UnloadModel(m_grandstandsModel);
    if (m_cityModel.meshCount > 0)        UnloadModel(m_cityModel);
}

void Renderer::DoBloom() {}

} // namespace Render
} // namespace F1Game

