#include "ShaderManager.hpp"

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <algorithm>

namespace F1Game {
namespace render {

ShaderManager::ShaderManager() = default;

ShaderManager::~ShaderManager() {
    UnloadAll();
}

bool ShaderManager::CompileShader(ShaderHandle& handle, const std::string& vertPath, const std::string& fragPath) {
    handle.shader = ::LoadShader(vertPath.c_str(), fragPath.c_str());
    handle.loaded = (handle.shader.id != 0);
    if (!handle.loaded) {
        TraceLog(LOG_ERROR, "Failed to load shader: %s | %s", vertPath.c_str(), fragPath.c_str());
    }
    return handle.loaded;
}

bool ShaderManager::LoadPBRShader() {
    return CompileShader(m_pbrShader, "assets/shaders/textshaders/pbr.vert", "assets/shaders/textshaders/pbr.frag");
}

bool ShaderManager::LoadBloomShader() {
    // Extract bright areas for bloom
    return CompileShader(m_bloomShader, "assets/shaders/textshaders/base.vert", "assets/shaders/textshaders/bloom_bright.frag");
}

bool ShaderManager::LoadMotionBlurShader() {
    return CompileShader(m_motionBlurShader, "assets/shaders/textshaders/base.vert", "assets/shaders/textshaders/motion_blur.frag");
}

void ShaderManager::SetPBRUniforms(const PBRMaterial& material) {
    if (!m_pbrShader.loaded) return;
    // PBR uniform wiring can be added here later.
}

bool ShaderManager::LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
    ShaderHandle& handle = m_shaders[name];
    bool success = CompileShader(handle, vertPath, fragPath);
    handle.loaded = success;
    return success;
}

Shader* ShaderManager::GetShader(const std::string& name) {
    auto it = m_shaders.find(name);
    if (it == m_shaders.end() || !it->second.loaded) {
        return nullptr;
    }
    return &it->second.shader;
}

void ShaderManager::UnloadAll() {
    // Unload named shaders
    for (auto& kv : m_shaders) {
        if (kv.second.loaded) {
            UnloadShader(kv.second.shader);
            kv.second.loaded = false;
        }
    }
    m_shaders.clear();

    // Unload built-in handles
    if (m_pbrShader.loaded) {
        UnloadShader(m_pbrShader.shader);
        m_pbrShader.loaded = false;
    }
    if (m_bloomShader.loaded) {
        UnloadShader(m_bloomShader.shader);
        m_bloomShader.loaded = false;
    }
    if (m_motionBlurShader.loaded) {
        UnloadShader(m_motionBlurShader.shader);
        m_motionBlurShader.loaded = false;
    }
}

} // namespace render
} // namespace F1Game
