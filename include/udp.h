/* 
 * MIT License
 * 
 * Copyright (c) 2025 Jonny Röker
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
 * 
 * File:   udp.h
 * Author: jonny
 *
 * Created on 31. Mai 2025
 */

#ifndef UDP_H
#define UDP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int setup_udp_client_socket(const char* remote_ip, int port); // zum Senden
int setup_udp_server_socket(int port);                 // zum Empfangen

int send_udp_packet(int sockfd, const uint8_t* data, int len);
int recv_udp_packet(int sockfd, uint8_t* buffer, int maxlen);

#ifdef __cplusplus
}
#endif

#endif /* UDP_H */
