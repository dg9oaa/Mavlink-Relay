#include "udp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int setup_udp_client_socket(const char* remote_ip, int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, remote_ip, &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr)); // "fixierte" Verbindung
    return sock;
}

int setup_udp_server_socket(int listen_port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(listen_port);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    return sock;
}

int send_udp_packet(int sockfd, const uint8_t* data, int len) {
    return send(sockfd, data, len, 0);
}

int recv_udp_packet(int sockfd, uint8_t* buffer, int maxlen) {
    return recv(sockfd, buffer, maxlen, MSG_DONTWAIT); // non-blocking
}
