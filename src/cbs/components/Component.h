#ifndef Component_h
#define Component_h

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

class MyObject;
class ConnectionPipe;
class MessageManager;

class Component {
    friend class MyObject; // this means that Objects can access private members of this class

public:
    Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;
    virtual ~Component() = default;

    std::uint8_t ID() const { return m_ID; }
    MyObject& Object() const { return *m_Object; }

protected:
    virtual void Initialize() {};
    virtual void Update() {};
    virtual void Destroy() {};
    virtual void MakeConnectors(MessageManager& message_manager) { (void)message_manager; };

    void RegisterUpdateCall() const;
    void UnregisterUpdateCall() const;

private:
    class MyObject* m_Object{nullptr };
    std::uint8_t m_ID{ 0 };
};

#endif
