#include "Scene.h"

#include "../rendering/Drawable.h"
#include "../rendering/ILightSource.h"

void MyScene::PreRun() {
    m_Running = true;
    m_DrawManager.Initialize();
}

void MyScene::Run() {
    m_ObjectManager.InitializeObjects();

    // Initialize Time manager as close to game loop as possible
    // to avoid misrepresented delta time
    g_Time.Initialize();
    
    // Game loop
    while (m_Running && !glfwWindowShouldClose(g_Window)) {
        // If frame rate is greater than limit then wait
        do {
            g_Time.Hold();
            glfwPollEvents();
        } while (g_Time.DeltaTime() < m_FrameRateLimit);
        
        // Update global systems
        g_Time.Update();
        g_Input.Update(g_Window);
        
        // Managers
        m_ObjectManager.ProcessFrame();
        m_DrawManager.CallDraws();
    }
}

void MyScene::PostRun() {
    m_ObjectManager.DestroyObjects();
}

void MyScene::Exit() {
    m_Running = false;
}

void MyScene::FrameRateLimit(unsigned int frame_rate) {
    m_FrameRateLimit = frame_rate != 0 ? 1.0f / (float)frame_rate : 0.0f;
}

MyObject* MyScene::CreateObject(std::string name) {
    return m_ObjectManager.CreateObject(name);
}

void MyScene::DestroyObject(std::uint8_t id) {
    m_ObjectManager.DestroyObject(id);
}

void MyScene::RegisterDrawCall(Drawable* drawable) {
    m_DrawManager.RegisterDrawCall(drawable);
}

void MyScene::UnregisterDrawCall(Drawable* drawable) {
    m_DrawManager.UnregisterDrawCall(drawable);
}

void MyScene::RegisterWidget(IWidget* widget) {
    m_DrawManager.RegisterWidget(widget);
}

void MyScene::UnregisterWidget(IWidget* widget) {
    m_DrawManager.UnregisterWidget(widget);
}

void MyScene::RegisterLightSource(ILightSource* light_source) {
    m_DrawManager.RegisterLightSource(light_source);
}

void MyScene::UnregisterLightSource(ILightSource* light_source) {
    m_DrawManager.UnregisterLightSource(light_source);
}

void MyScene::RegisterCamera(Camera* camera) {
    m_DrawManager.RegisterCamera(camera);
}

Camera* MyScene::MainCamera() const {
    return m_DrawManager.MainCamera();
}

void MyScene::Skybox(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& back, const std::string& front) {
    m_DrawManager.Skybox(right, left, top, bottom, back, front);
}

void MyScene::Background(const glm::vec3& background) {
    m_DrawManager.Background(background);
}
