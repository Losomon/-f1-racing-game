#pragma once

// Forward declarations
namespace F1Game {
    class Car;
    class Track;
}
namespace F1Game { namespace render {
    class ShaderManager;
}}

#include <raylib.h>
#include <memory>

namespace F1Game {
namespace Render {

class Renderer {
public:
    Renderer();
    ~Renderer();
    void Init(int width, int height);
    void Shutdown();
    void InitPostFX();
    void ToggleScreenshotMode();
    void RenderToHDR();
    void DoPostProcessing();
    void RenderScene();
    void BeginFrame();
    void EndFrame();
    void RenderUI();
    void DrawCar(const F1Game::Car& car);
    void DrawTrack(const F1Game::Track& track);
    void DrawEnvironment();
    void UpdateCamera(float dt);
    void UpdateFollowCamera();
    void UpdateOrbitCamera();
    void DrawDebugGrid();
    void DrawSkybox();
    void SetVignette(float intensity);
    void SetColorGrading(const char* lutPath);
    void UnloadModels();
    void DoBloom();
    void SetCameraMode(CameraMode mode);
    void SetTargetCar(F1Game::Car* car);
    void SetTrack(F1Game::Track* track);
    enum class CameraMode {
        Follow = 0,
        TV,
        Drone,
        Cinematic,
        Orbit
    };

private:
    Camera3D m_camera = {0};
    Model m_carModel = {0};
    Model m_trackModel = {0};
    Model m_grandstandsModel = {0};
    Model m_cityModel = {0};
    int m_width = 0;
    int m_height = 0;
    bool m_initialized = false;
    bool m_screenshotMode = false;
    bool m_motionBlurEnabled = true;
    RenderTexture2D m_sceneHDR = {0};
    RenderTexture2D m_velocityTex = {0};
    RenderTexture2D m_bloomPyramid[6] = {0};
    std::unique_ptr<F1Game::render::ShaderManager> m_shaderManager;
    F1Game::Car* m_targetCar = nullptr;
    F1Game::Track* m_track = nullptr;
    CameraMode m_cameraMode = CameraMode::Follow;
};

} // namespace Render
} // namespace F1Game

