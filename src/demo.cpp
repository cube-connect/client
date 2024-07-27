#include <iostream>
#include <enet/enet.h>
#include <cstring>

// ./enet_client <server_ip> <message>
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <message>\n";
        return EXIT_FAILURE;
    }

    const char* server_ip = argv[1];
    const char* message = argv[2];

    if (enet_initialize() != 0) {
        std::cerr << "An error occurred while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    ENetHost* client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (client == nullptr) {
        std::cerr << "An error occurred while trying to create an ENet client host.\n";
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, server_ip);
    address.port = 7777; // TODO: param 7777

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == nullptr) {
        std::cerr << "No available peers for initiating an ENet connection.\n";
        return EXIT_FAILURE;
    }

    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection to " << server_ip << ":1234 succeeded.\n";
    } else {
        enet_peer_reset(peer);
        std::cerr << "Connection to " << server_ip << ":1234 failed.\n";
        return EXIT_FAILURE;
    }

    // Sending data to the server
    ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);

    // Event loop
    bool running = true;
    while (running) {
        // on each iteration of this outer while loop, the buffer of packets will be empty by the end?
        // practically, the time would be 1 / freq per iteration
        // if client and server freqs are the same, it'll be about 1 packet per iteration

        while (enet_host_service(client, &event, 0) > 0) { // send and receive
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout << "A packet of length "
                              << event.packet->dataLength
                              << " containing '"
                              << event.packet->data
                              << "' was received from "
                              << event.peer->data
                              << " on channel "
                              << event.channelID
                              << ".\n";
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << event.peer->data << " disconnected.\n";
                    event.peer->data = nullptr;
                    running = false;
                    break;
            }
        }

        // Send data to all connected peers at the end of the service loop
        const char* message = "Server message at the end of the service loop";
        ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);

        enet_peer_send(peer, 0, packet);


        enet_host_flush(server);

        // sleep
    }

    enet_host_destroy(client);
    return EXIT_SUCCESS;
}
