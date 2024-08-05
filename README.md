# client

We created a basic working client-server UDP setup using ENet and defined the 
data types we'll be sending back and forth between the client and server.

Our next step is to:
1. Client: Figure out how to get a window up and running. And then using server
           data (potentially dummy data at first) and rendering that. Also
           need to install other packages. GLFW, GLM, etc.
2. Server: Looking at the cube_connect code and extracting what needs to go on
           the server. Use the drawing_snapshot.hpp to help figure that out.


7/07/24
We copy pasted the cube_connect code into the client and server repos. We added
a network loop with ENet and began refactoring the app to use data sent/received
over the network.

For client:
* implemented matrices for cubies and camera (NetworkDraw for Drawables)
* TODO: text for TextRenderer (NetworkDraw for IWidget)
* TODO: send over InputSnapshot after populating with keypresses/mouse (potential idea
  in comments in Input.cpp)

2024-08-05
Populated and sent InputSnapshot. Got working client and server over network.

TODO:
* Implement some sort of lock to ensure only one client can control the cube at a time.
* Reimplement DrawingSnapshot to use dynamic size vector instead of array.
* TextRenderer
* (later) simplify codebase






#include <enet/enet.h>
#include <iostream>
#include <vector>
#include <array>
#include <cstring>

struct DrawingSnapshot {
    std::array<float, 3> camera_pos;
    std::array<float, 16> world_to_camera; // view matrix
    std::array<float, 16> camera_to_clip;  // projection matrix
    std::vector<std::array<float, 16>> local_to_world_matrices; // TODO: put in unique ID to identify object
};

std::vector<uint8_t> serialize_snapshot(const DrawingSnapshot& snapshot) {
    size_t size = sizeof(snapshot.camera_pos) + sizeof(snapshot.world_to_camera) + sizeof(snapshot.camera_to_clip) +
                  sizeof(size_t) + snapshot.local_to_world_matrices.size() * sizeof(std::array<float, 16>);

    std::vector<uint8_t> buffer(size);
    uint8_t* ptr = buffer.data();

    std::memcpy(ptr, snapshot.camera_pos.data(), sizeof(snapshot.camera_pos));
    ptr += sizeof(snapshot.camera_pos);

    std::memcpy(ptr, snapshot.world_to_camera.data(), sizeof(snapshot.world_to_camera));
    ptr += sizeof(snapshot.world_to_camera);

    std::memcpy(ptr, snapshot.camera_to_clip.data(), sizeof(snapshot.camera_to_clip));
    ptr += sizeof(snapshot.camera_to_clip);

    size_t matrix_count = snapshot.local_to_world_matrices.size();
    std::memcpy(ptr, &matrix_count, sizeof(size_t));
    ptr += sizeof(size_t);

    if (matrix_count > 0) {
        std::memcpy(ptr, snapshot.local_to_world_matrices.data(), matrix_count * sizeof(std::array<float, 16>));
    }

    return buffer;
}

DrawingSnapshot deserialize_snapshot(const uint8_t* data, size_t length) {
    DrawingSnapshot snapshot;
    const uint8_t* ptr = data;

    std::memcpy(snapshot.camera_pos.data(), ptr, sizeof(snapshot.camera_pos));
    ptr += sizeof(snapshot.camera_pos);

    std::memcpy(snapshot.world_to_camera.data(), ptr, sizeof(snapshot.world_to_camera));
    ptr += sizeof(snapshot.world_to_camera);

    std::memcpy(snapshot.camera_to_clip.data(), ptr, sizeof(snapshot.camera_to_clip));
    ptr += sizeof(snapshot.camera_to_clip);

    size_t matrix_count;
    std::memcpy(&matrix_count, ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    snapshot.local_to_world_matrices.resize(matrix_count);
    if (matrix_count > 0) {
        std::memcpy(snapshot.local_to_world_matrices.data(), ptr, matrix_count * sizeof(std::array<float, 16>));
    }

    return snapshot;
}

void receive_snapshot(const ENetPacket* packet) {
    const uint8_t* data = packet->data;
    size_t length = packet->dataLength;

    DrawingSnapshot snapshot = deserialize_snapshot(data, length);

    // Use the received snapshot (for example, print some values)
    std::cout << "Camera Position: (" << snapshot.camera_pos[0] << ", " << snapshot.camera_pos[1] << ", " << snapshot.camera_pos[2] << ")\n";
    std::cout << "Number of Local to World Matrices: " << snapshot.local_to_world_matrices.size() << "\n";
}

int main(int argc, char** argv) {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost* server;

    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == nullptr) {
        std::cerr << "Failed to create ENet server\n";
        enet_deinitialize();
        return EXIT_FAILURE;
    }

    while (true) {
        ENetEvent event;
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "A new client connected from " << event.peer->address.host << ":" << event.peer->address.port << "\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    receive_snapshot(event.packet);
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected\n";
                    break;

                default:
                    break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();

    return 0;
}

