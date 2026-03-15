#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

namespace F1Game {

// Event base class
struct Event {
    virtual ~Event() = default;
};

// Event listener function type
using EventHandler = std::function<void(std::shared_ptr<Event>)>;

// Event Bus for decoupled messaging
class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;

    // Subscribe to an event type
    template<typename T>
    void Subscribe(EventHandler handler) {
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");
        auto type = std::type_index(typeid(T));
        m_handlers[type].push_back(handler);
    }

    // Publish an event
    template<typename T, typename... Args>
    void Publish(Args&&... args) {
        static_assert(std::is_base_of<Event, T>::value, "T must derive from Event");
        auto event = std::make_shared<T>(std::forward<Args>(args)...);
        auto type = std::type_index(typeid(T));
        
        auto it = m_handlers.find(type);
        if (it != m_handlers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    // Clear all handlers
    void Clear() {
        m_handlers.clear();
    }

private:
    std::unordered_map<std::type_index, std::vector<EventHandler>> m_handlers;
};

// Common events for the game
struct CollisionEvent : Event {
    int entityA;
    int entityB;
    Vector3 contactPoint;
    Vector3 normal;
};

struct InputEvent : Event {
    int key;
    bool pressed;
    bool held;
};

struct GameStateEvent : Event {
    enum class State {
        MainMenu,
        Racing,
        Paused,
        Finished
    };
    State newState;
    State oldState;
};

} // namespace F1Game


