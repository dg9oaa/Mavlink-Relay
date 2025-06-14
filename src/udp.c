/* 
 * MIT License
 * 
 * Copyright (c) 2025 Jonny Roeker
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * File:   udp.c
 * Author: jonny
 *
 * Created on 31. Mai 2025
 */

#include "udp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

extern char *progname;
pthread_mutex_t lock_udp;

int setup_udp_client_socket(const char* remote_ip, int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, remote_ip, &addr.sin_addr);

    if (pthread_mutex_init(&lock_udp, NULL) != 0) {
        fprintf(stderr, "%s: create a mutex failed\n", progname);
        return -1;
    }

    connect(sock, (struct sockaddr*)&addr, sizeof(addr)); // "fixierte" Verbindung
    return sock;
}

int setup_udp_server_socket(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (pthread_mutex_init(&lock_udp, NULL) != 0) {
        fprintf(stderr, "%s: create a mutex failed\n", progname);
        return -1;
    }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    return sock;
}

int send_udp_packet(int sockfd, const uint8_t* data, int len) {
    pthread_mutex_lock(&lock_udp);
    int bytesWrite = send(sockfd, data, len, 0);
    pthread_mutex_unlock(&lock_udp);
    return bytesWrite;
}

int recv_udp_packet(int sockfd, uint8_t* buffer, int maxlen) {
    pthread_mutex_lock(&lock_udp);
    int bytesRead = recv(sockfd, buffer, maxlen, MSG_DONTWAIT); // non-blocking
    pthread_mutex_unlock(&lock_udp);
    return bytesRead;
}
