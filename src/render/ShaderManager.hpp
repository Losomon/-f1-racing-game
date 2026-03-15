#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace F1Game {
namespace render {

// Shader wrapper
struct ShaderHandle {
    Shader shader;
    bool loaded = false;
};

// PBR Material properties
struct PBRMaterial {
    Color albedo = WHITE;
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float normalStrength = 1.0f;
};

// Shader Manager for loading and managing custom shaders
class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    // Load shaders
    bool LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
    bool LoadPBRShader();
    bool LoadBloomShader();
    bool LoadMotionBlurShader();

    // Get shader
    Shader* GetShader(const std::string& name);
    Shader* GetPBRShader() { return &m_pbrShader.shader; }
    Shader* GetBloomShader() { return &m_bloomShader.shader; }
    Shader* GetMotionBlurShader() { return &m_motionBlurShader.shader; }

    // Set uniforms
    void SetPBRUniforms(const PBRMaterial& material);

    // Cleanup
    void UnloadAll();

private:
    std::unordered_map<std::string, ShaderHandle> m_shaders;
    ShaderHandle m_pbrShader;
    ShaderHandle m_bloomShader;
    ShaderHandle m_motionBlurShader;

    // Internal
    bool CompileShader(ShaderHandle& handle, const std::string& vertPath, const std::string& fragPath);
};

} // namespace render
} // namespace F1Game


