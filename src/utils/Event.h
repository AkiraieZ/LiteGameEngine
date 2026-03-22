#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <typeindex>
#include <typeinfo>
#include <algorithm>
#include <queue>
#include <thread>
#include <condition_variable>

namespace LGE {

class Event {
public:
    virtual ~Event() = default;
    virtual const char* getName() const = 0;
};

template<typename T>
class EventImpl : public Event {
public:
    const char* getName() const override {
        static const char* name = typeid(T).name();
        return name;
    }
};

class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    virtual void handle(Event* event) = 0;
    virtual const std::type_info& getEventType() const = 0;
};

template<typename T>
class EventHandler : public IEventHandler {
public:
    using Callback = std::function<void(T&)>;

    explicit EventHandler(Callback callback) : m_callback(std::move(callback)) {}

    void handle(Event* event) override {
        T* typedEvent = static_cast<T*>(event);
        m_callback(*typedEvent);
    }

    const std::type_info& getEventType() const override {
        return typeid(T);
    }

private:
    Callback m_callback;
};

class ListenerHandle {
public:
    ListenerHandle() : m_id(0), m_eventBus(nullptr) {}
    ListenerHandle(uint64_t id, void* eventBus) : m_id(id), m_eventBus(eventBus) {}

    bool isValid() const { return m_id != 0; }
    uint64_t getId() const { return m_id; }
    void* getEventBus() const { return m_eventBus; }

    void reset() {
        m_id = 0;
        m_eventBus = nullptr;
    }

    operator bool() const { return isValid(); }

private:
    uint64_t m_id;
    void* m_eventBus;
};

class EventBus {
public:
    EventBus() : m_nextId(1) {}
    ~EventBus() = default;

    template<typename T>
    ListenerHandle subscribe(std::function<void(T&)> callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        uint64_t id = m_nextId++;
        auto handler = std::make_unique<EventHandler<T>>(std::move(callback));
        m_handlers[typeid(T)].emplace_back(id, std::move(handler));
        m_handleMap[id] = typeid(T);
        return ListenerHandle(id, this);
    }

    template<typename T, typename... Args>
    ListenerHandle subscribe(void (*func)(T&, Args...), Args... args) {
        return subscribe<T>([func, args...](T& event) {
            func(event, args...);
        });
    }

    template<typename T, typename Class, typename... Args>
    ListenerHandle subscribe(Class* instance, void (Class::*method)(T&, Args...), Args... args) {
        return subscribe<T>([instance, method, args...](T& event) {
            (instance->*method)(event, args...);
        });
    }

    bool unsubscribe(ListenerHandle handle) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!handle.isValid() || handle.getEventBus() != this) {
            return false;
        }

        auto it = m_handleMap.find(handle.getId());
        if (it == m_handleMap.end()) {
            return false;
        }

        auto& handlers = m_handlers[it->second];
        handlers.erase(
            std::remove_if(handlers.begin(), handlers.end(),
                [handle](const auto& pair) { return pair.first == handle.getId(); }),
            handlers.end()
        );

        m_handleMap.erase(it);
        return true;
    }

    void unsubscribeAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_handlers.clear();
        m_handleMap.clear();
    }

    template<typename T>
    void publish(T& event) {
        std::vector<std::pair<uint64_t, std::unique_ptr<IEventHandler>>> handlersCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_handlers.find(typeid(T));
            if (it != m_handlers.end()) {
                for (const auto& [id, handler] : it->second) {
                    handlersCopy.emplace_back(id, nullptr);
                    handlersCopy.back().second.reset(handler.get());
                }
            }
        }

        for (auto& [id, handler] : handlersCopy) {
            if (handler) {
                handler->handle(&event);
            }
        }
    }

    template<typename T>
    void publish(const T& event) {
        T copy = event;
        publish(copy);
    }

    template<typename T, typename... Args>
    void publish(Args&&... args) {
        T event(std::forward<Args>(args)...);
        publish(event);
    }

    size_t getListenerCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_handleMap.size();
    }

    template<typename T>
    size_t getListenerCountForType() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(typeid(T));
        if (it != m_handlers.end()) {
            return it->second.size();
        }
        return 0;
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::type_index, std::vector<std::pair<uint64_t, std::unique_ptr<IEventHandler>>>> m_handlers;
    std::unordered_map<uint64_t, std::type_index> m_handleMap;
    uint64_t m_nextId;
};

class EventQueue {
public:
    EventQueue() : m_running(false) {}

    ~EventQueue() {
        stop();
    }

    void start() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_running) return;
        m_running = true;
        m_worker = std::thread(&EventQueue::processLoop, this);
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_running) return;
            m_running = false;
        }
        m_cv.notify_all();
        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    bool isRunning() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_running;
    }

    template<typename T>
    void enqueue(T&& event) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_events.push(std::make_unique<EventWrapper<std::decay_t<T>>>(std::forward<T>(event)));
        }
        m_cv.notify_one();
    }

    template<typename T, typename... Args>
    void enqueue(Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_events.push(std::make_unique<EventWrapper<T>>(std::forward<Args>(args)...));
        }
        m_cv.notify_one();
    }

    void process() {
        std::queue<std::unique_ptr<IEventWrapper>> localEvents;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(m_events, localEvents);
        }

        while (!localEvents.empty()) {
            auto& event = localEvents.front();
            event->dispatch();
            localEvents.pop();
        }
    }

    size_t getPendingEventCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_events.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<std::unique_ptr<IEventWrapper>> empty;
        std::swap(m_events, empty);
    }

    template<typename T>
    void subscribe(std::function<void(T&)> callback) {
        m_eventBus.subscribe<T>(std::move(callback));
    }

    template<typename T>
    ListenerHandle subscribeWithHandle(std::function<void(T&)> callback) {
        return m_eventBus.subscribe<T>(std::move(callback));
    }

    bool unsubscribe(ListenerHandle handle) {
        return m_eventBus.unsubscribe(handle);
    }

private:
    struct IEventWrapper {
        virtual ~IEventWrapper() = default;
        virtual void dispatch() = 0;
    };

    template<typename T>
    struct EventWrapper : IEventWrapper {
        T event;

        template<typename... Args>
        explicit EventWrapper(Args&&... args) : event(std::forward<Args>(args)...) {}

        void dispatch() override {
            m_eventBus.publish(event);
        }
    };

    void processLoop() {
        while (true) {
            std::unique_ptr<IEventWrapper> event;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() {
                    return !m_running || !m_events.empty();
                });

                if (!m_running && m_events.empty()) {
                    break;
                }

                if (!m_events.empty()) {
                    event = std::move(m_events.front());
                    m_events.pop();
                }
            }

            if (event) {
                event->dispatch();
            }
        }
    }

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<std::unique_ptr<IEventWrapper>> m_events;
    EventBus m_eventBus;
    std::thread m_worker;
    bool m_running;
};

class ScopedListener {
public:
    ScopedListener() = default;

    ScopedListener(ListenerHandle handle) : m_handle(handle) {}

    ScopedListener(ScopedListener&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle.reset();
    }

    ScopedListener& operator=(ScopedListener&& other) noexcept {
        if (this != &other) {
            unsubscribe();
            m_handle = other.m_handle;
            other.m_handle.reset();
        }
        return *this;
    }

    ~ScopedListener() {
        unsubscribe();
    }

    ScopedListener(const ScopedListener&) = delete;
    ScopedListener& operator=(const ScopedListener&) = delete;

    void unsubscribe() {
        if (m_handle) {
            auto* bus = static_cast<EventBus*>(m_handle.getEventBus());
            if (bus) {
                bus->unsubscribe(m_handle);
            }
            m_handle.reset();
        }
    }

    operator bool() const { return m_handle.isValid(); }

private:
    ListenerHandle m_handle;
};

template<typename T>
class Signal {
public:
    using Slot = std::function<void(T)>;
    using Connection = uint64_t;

    Connection connect(Slot slot) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Connection id = m_nextId++;
        m_slots[id] = std::move(slot);
        return id;
    }

    template<typename Class, typename Method>
    Connection connect(Class* instance, Method method) {
        return connect([instance, method](T value) {
            (instance->*method)(value);
        });
    }

    bool disconnect(Connection id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_slots.erase(id) > 0;
    }

    void disconnectAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.clear();
    }

    void emit(T value) {
        std::vector<Slot> slotsCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& [id, slot] : m_slots) {
                slotsCopy.push_back(slot);
            }
        }

        for (const auto& slot : slotsCopy) {
            slot(value);
        }
    }

    void operator()(T value) {
        emit(value);
    }

    size_t getConnectionCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_slots.size();
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<Connection, Slot> m_slots;
    Connection m_nextId = 1;
};

template<>
class Signal<void> {
public:
    using Slot = std::function<void()>;
    using Connection = uint64_t;

    Connection connect(Slot slot) {
        std::lock_guard<std::mutex> lock(m_mutex);
        Connection id = m_nextId++;
        m_slots[id] = std::move(slot);
        return id;
    }

    template<typename Class, typename Method>
    Connection connect(Class* instance, Method method) {
        return connect([instance, method]() {
            (instance->*method)();
        });
    }

    bool disconnect(Connection id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_slots.erase(id) > 0;
    }

    void disconnectAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.clear();
    }

    void emit() {
        std::vector<Slot> slotsCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (const auto& [id, slot] : m_slots) {
                slotsCopy.push_back(slot);
            }
        }

        for (const auto& slot : slotsCopy) {
            slot();
        }
    }

    void operator()() {
        emit();
    }

    size_t getConnectionCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_slots.size();
    }

private:
    mutable std::mutex m_mutex;
    std::unordered_map<Connection, Slot> m_slots;
    Connection m_nextId = 1;
};

class ScopedConnection {
public:
    using DisconnectFunc = std::function<void()>;

    ScopedConnection() = default;

    ScopedConnection(DisconnectFunc disconnectFunc)
        : m_disconnectFunc(std::move(disconnectFunc)) {}

    ScopedConnection(ScopedConnection&& other) noexcept
        : m_disconnectFunc(std::move(other.m_disconnectFunc)) {
        other.m_disconnectFunc = nullptr;
    }

    ScopedConnection& operator=(ScopedConnection&& other) noexcept {
        if (this != &other) {
            disconnect();
            m_disconnectFunc = std::move(other.m_disconnectFunc);
            other.m_disconnectFunc = nullptr;
        }
        return *this;
    }

    ~ScopedConnection() {
        disconnect();
    }

    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;

    void disconnect() {
        if (m_disconnectFunc) {
            m_disconnectFunc();
            m_disconnectFunc = nullptr;
        }
    }

    operator bool() const { return m_disconnectFunc != nullptr; }

private:
    DisconnectFunc m_disconnectFunc;
};

}
