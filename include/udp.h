#pragma once

#include <stdint.h>

int setup_udp_client_socket(const char* remote_ip, int port); // zum Senden
int setup_udp_server_socket(int listen_port);                 // zum Empfangen

int send_udp_packet(int sockfd, const uint8_t* data, int len);
int recv_udp_packet(int sockfd, uint8_t* buffer, int maxlen);

