#ifndef ObjectManager_h
#define ObjectManager_h

#include "Object.h"

#include <vector>
#include <string>

class MyScene;

class ObjectManager {
    using Objects_t = std::vector<std::unique_ptr<MyObject>>;

public:
    explicit ObjectManager(class MyScene& owner);

    void ProcessFrame();

    void InitializeObjects();
    void UpdateObjects();
    void DestroyObjects();
    
    MyObject* CreateObject(std::string name = "");
    void DestroyObject(std::uint8_t id);
    
    MyScene& Scene() const { return m_Scene; }
    
private:
    void MarkToDestroy(Objects_t::iterator it);

    class MyScene& m_Scene;

    std::uint8_t m_NextObjectID;
    Objects_t m_Objects;
    Objects_t::size_type m_ToInitialize;
    Objects_t::size_type m_ToInitializeNextFrame;
    Objects_t::size_type m_ToDestroy;
};

#endif
