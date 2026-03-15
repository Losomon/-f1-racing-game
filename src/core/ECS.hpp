#pragma once

#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>
#include <string>

namespace F1Game {

// Entity is just an ID
using Entity = uint32_t;
const Entity INVALID_ENTITY = 0;

// Component base class
struct Component {
    virtual ~Component() = default;
};

// Tag component for identifying entities
struct TagComponent : Component {
    std::string tag;
    TagComponent() = default;
    explicit TagComponent(const std::string& t) : tag(t) {}
};

// Transform component (position, rotation, scale)
struct TransformComponent : Component {
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};  // Euler angles in degrees
    Vector3 scale = {1, 1, 1};
};

// Signature for system queries
using ComponentSignature = std::unordered_map<std::type_index, bool>;

// System base class
class System {
public:
    virtual ~System() = default;
    virtual void Update(double dt) = 0;
    virtual void Render(double alpha) = 0;
};

// ECS main class
class ECS {
public:
    ECS();
    ~ECS() = default;

    // Entity management
    Entity CreateEntity();
    void DestroyEntity(Entity e);
    bool IsValid(Entity e) const;

    // Tag component
    TagComponent* AddTag(Entity e, const std::string& tag);
    TagComponent* GetTag(Entity e);
    bool HasTag(Entity e, const std::string& tag) const;

    // Transform component
    TransformComponent* AddTransform(Entity e);
    TransformComponent* GetTransform(Entity e);
    bool HasTransform(Entity e) const;

    // Generic component management
    template<typename T>
    T* AddComponent(Entity e) {
        if (!IsValid(e)) return nullptr;
        
        auto type = std::type_index(typeid(T));
        if (m_components[type].find(e) == m_components[type].end()) {
            m_components[type][e] = std::make_unique<T>();
        }
        m_signatures[e][type] = true;
        return static_cast<T*>(m_components[type][e].get());
    }

    template<typename T>
    T* GetComponent(Entity e) {
        if (!IsValid(e)) return nullptr;
        
        auto type = std::type_index(typeid(T));
        auto it = m_components[type].find(e);
        if (it != m_components[type].end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    bool HasComponent(Entity e) const {
        if (!IsValid(e)) return false;
        auto type = std::type_index(typeid(T));
        auto it = m_signatures.at(e).find(type);
        return it != m_signatures.at(e).end() && it->second;
    }

    // System management
    template<typename T>
    void AddSystem(std::unique_ptr<T> system) {
        static_assert(std::is_base_of<System, T>::value, "T must derive from System");
        m_systems.push_back(std::move(system));
    }

    void UpdateSystems(double dt);
    void RenderSystems(double alpha);

    // Query entities by component types
    template<typename... T>
    std::vector<Entity> Query() {
        std::vector<Entity> result;
        for (const auto& [entity, signature] : m_signatures) {
            bool match = (signature.at(std::type_index(typeid(T))) && ...);
            if (match) {
                result.push_back(entity);
            }
        }
        return result;
    }

private:
    Entity m_nextEntity = 1;
    std::unordered_map<Entity, ComponentSignature> m_signatures;
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::unique_ptr<Component>>> m_components;
    std::vector<std::unique_ptr<System>> m_systems;
};

// Inline implementations
inline ECS::ECS() {
    m_signatures.reserve(100);
}

inline Entity ECS::CreateEntity() {
    Entity e = m_nextEntity++;
    m_signatures[e] = {};
    return e;
}

inline void ECS::DestroyEntity(Entity e) {
    if (!IsValid(e)) return;
    
    m_signatures.erase(e);
    for (auto& [type, components] : m_components) {
        components.erase(e);
    }
}

inline bool ECS::IsValid(Entity e) const {
    return e != INVALID_ENTITY && m_signatures.find(e) != m_signatures.end();
}

inline TagComponent* ECS::AddTag(Entity e, const std::string& tag) {
    auto* t = AddComponent<TagComponent>(e);
    t->tag = tag;
    return t;
}

inline TagComponent* ECS::GetTag(Entity e) {
    return GetComponent<TagComponent>(e);
}

inline bool ECS::HasTag(Entity e, const std::string& tag) const {
    auto* t = const_cast<ECS*>(this)->GetComponent<TagComponent>(e);
    return t && t->tag == tag;
}

inline TransformComponent* ECS::AddTransform(Entity e) {
    return AddComponent<TransformComponent>(e);
}

inline TransformComponent* ECS::GetTransform(Entity e) {
    return GetComponent<TransformComponent>(e);
}

inline bool ECS::HasTransform(Entity e) const {
    return HasComponent<TransformComponent>(e);
}

inline void ECS::UpdateSystems(double dt) {
    for (auto& system : m_systems) {
        system->Update(dt);
    }
}

inline void ECS::RenderSystems(double alpha) {
    for (auto& system : m_systems) {
        system->Render(alpha);
    }
}

} // namespace F1Game


