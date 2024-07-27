#ifndef MessageIn_h
#define MessageIn_h

#include "MessageManager.h"

// This defines the third template parameter F as a pointer to a member function of class O that takes an argument of type M and returns void.
template <class M, class O, void(O::*F)(M)>
class MessageIn final : public AbstractMessageIn {
    friend class MessageManager;

public:
    MessageIn() 
        : m_Owner(nullptr) { }
    ~MessageIn() = default;

    Component* Owner() const override { return m_Owner; }

    void Receive(void* message) override { (m_Owner->*F)(*static_cast<M*>(message)); }

private:
    MessageIn(O* owner)
        : m_Owner(owner) {}

    MessageIn& operator=(const MessageIn& other) {
        if (this == &other) {
            return *this;
        }

        m_Owner = other.m_Owner;

        return *this;
    }

    MessageIn(const MessageIn&) = default;
    MessageIn(MessageIn&&) = default;
    MessageIn& operator=(MessageIn&&) = default;

    O* m_Owner;
};

#endif
