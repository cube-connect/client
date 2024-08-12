#include "Scene.h"
#include <enet/enet.h>

#include <iostream>

#include "../client_server_shared/drawing_snapshot.hpp"
#include "../client_server_shared/input_snapshot.hpp"

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
    
    // set up connection to server
    if (enet_initialize() != 0) {
        throw std::runtime_error("An error occurred while initializing ENet.");
    }
    atexit(enet_deinitialize);

    ENetHost* client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        throw std::runtime_error("An error occurred while trying to create an ENet client host.");
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    // TODO: make arguments to program
    // enet_address_set_host(&address, "localhost");
    enet_address_set_host(&address, "104.131.10.102");
    address.port = 7777; 

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        throw std::runtime_error("No available peers for initiating an ENet connection.");
    }

    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection to server succeeded.\n";
    } else {
        enet_peer_reset(peer);
        throw std::runtime_error("Connecting to server failed.");
    }

    // Get client ID assigned by server
    unsigned int client_id{};
    bool is_assigned = false;
    while (!is_assigned) {
        while (enet_host_service(client, &event, m_FrameRateLimit) > 0 && !is_assigned) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    memcpy(&client_id, event.packet->data, sizeof(unsigned int));
                    is_assigned = true;
                }
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    std::cout << event.peer->data << " disconnected.\n";
                    event.peer->data = nullptr;
                    Exit();
                }
                    break;
            }
        }
    }

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
        InputSnapshot input_snapshot = g_Input.NetworkUpdate(g_Window);
        input_snapshot.client_id = client_id;

        // std::cout << input_snapshot.enter_pressed << ' ' << input_snapshot.shift_pressed << std::endl;

        while (enet_host_service(client, &event, m_FrameRateLimit) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    DrawingSnapshot drawing_snapshot;
                    memcpy(&drawing_snapshot, event.packet->data, sizeof(DrawingSnapshot));
                    m_DrawManager.NetworkCallDraws(&drawing_snapshot);
                    enet_packet_destroy(event.packet);
                }
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    std::cout << event.peer->data << " disconnected.\n";
                    event.peer->data = nullptr;
                    Exit();
                }
                    break;
            }
        }

        // TODO: populate text
        ENetPacket* packet = enet_packet_create(&input_snapshot, sizeof(InputSnapshot), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);

        enet_host_flush(client);

        // m_ObjectManager.ProcessFrame(); 
        // m_DrawManager.CallDraws();
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
