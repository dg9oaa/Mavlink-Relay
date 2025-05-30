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
 * File:   serial.c
 * Author: jonny
 *
 * Created on 29. Mai 2025, 
 */

#include "serial.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>


extern char *progname;
extern bool debug;
pthread_mutex_t lock;

int openSerial(const char* device, int baudrate) {
    if (debug)
        printf("DEBUG: serial port %s try to open\n", device);

    int fd = open(device, O_RDWR | O_NOCTTY  | O_NDELAY);
    if (fd < 0) {
        fprintf(stderr, "%s: error to open device %s: %s\n", progname, device, strerror(errno));
        return -1;
    } else {
        fcntl(fd, F_SETFL, 0);
    }
    if (debug)
        printf("DEBUG: serial port %s opend\n", device);

    struct termios options;
    memset(&options, 0, sizeof(options));

    if (tcgetattr(fd, &options) < 0) {
        fprintf(stderr, "%s: could not read configuration of fd %d\n", progname, fd);
        return -1;
    }

    switch (baudrate) {
        case 9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options, B19200);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            break;
        case 57600:
            cfsetispeed(&options, B57600);
            cfsetospeed(&options, B57600);
            break;
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            fprintf(stderr, "ERROR: baudrate %d could not be set, aborting\n", baudrate);
            return -1;
            break;
                
    }
    printf("DEBUG: baudrate set to %d\n", baudrate);

/*
    options.c_cflag |= (CLOCAL | CREAD | CS8);
    options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
*/

/*
    options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= CS8;
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 10;
*/

    // Control Flags: 8N1, aktivieren Receiver, ignorieren Modemsteuerung
    options.c_cflag &= ~PARENB;                // Keine Parität
    options.c_cflag &= ~CSTOPB;                // 1 Stopbit
    options.c_cflag &= ~CSIZE;                 // Maskiere Datengrößen-Bits
    options.c_cflag |= CS8;                    // 8 Datenbits
    options.c_cflag |= CLOCAL | CREAD;         // Lokal, Empfang aktivieren

    // Input Flags: keine Umwandlungen, kein Flow Control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);          // keine SW Flow Control
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | INPCK);

    // Output Flags: kein \n → \r\n
    options.c_oflag &= ~OPOST;

    // Local Flags: kein Echo, keine Signale, kein Canonical Mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN);

    // Blocking Read: warte auf 1 Zeichen, kein Timeout
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

/*
    tcsetattr(fd, TCSANOW, &options);
*/
    if (tcsetattr(fd, TCSAFLUSH, &options) < 0) {
        fprintf(stderr, "%s: could not set configuration of fd %d\n", progname, fd);
        return -1;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "%s: create a mutex failed\n", progname);
        return -1;
    }

    return fd;
}

int readSerial(int fd, uint8_t* buffer, int buffer_size) {
    pthread_mutex_lock(&lock);
    int bytesRead = read(fd, buffer, buffer_size);
    pthread_mutex_unlock(&lock);
    return bytesRead;
}

int writeSerial(int fd, uint8_t* buffer, int len) {
    pthread_mutex_lock(&lock);
    int bytesWrite = write(fd, buffer, len);
    tcdrain(fd);
    pthread_mutex_unlock(&lock);
    return bytesWrite;
}

void closeSerial(int fd) {
    close(fd);
}

