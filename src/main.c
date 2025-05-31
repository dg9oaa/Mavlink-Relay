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
 * File:   main.c
 * Author: jonny
 *
 * Created on 30. Mai 2025, 14:26
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>

#include "serial.h"
#include "udp.h"
#include "option.h"
#include "common/mavlink.h"
#include "logging.h"


#define SERIAL_DEVICE "/dev/ttyACM0"
#define SERIAL_DEVICE_BAUDRATE 57600

#define UDP_IP "10.100.1.102"
#define UDP_PORT 14550

options_t options;
extern char *progname;


int main(int argc, char *argv[]) {

    getProgramName(argv);

    logSetLevel(WARN);
    options.level_dflt = "warn";
    options.level = options.level_dflt;

    options.device_dflt = SERIAL_DEVICE;
    options.baud_dflt = SERIAL_DEVICE_BAUDRATE;
    options.server = UDP_IP;
    options.port = UDP_PORT;

    parseOptions(argc, argv);

    logSTD();
    LOG__DEBUG("Teste Logging %s", options.level);
    LOG__INFO("Teste Logging %s", options.level);
    LOG__WARN("Teste Logging %s", options.level);
    LOG__ERROR("Teste Logging %s", options.level);
    
    
    if (options.daemon) {
        pid_t pid = fork();
        switch (pid) {
            case -1: /* Fehler */
                fprintf(stderr, "%s: error to fork(): %s.\n", progname, strerror(errno));
                exit(EXIT_FAILURE);
                break;
            case 0: /* Child process continues */
                break;
            default: /* Parent process terminated immediately */
                exit(EXIT_SUCCESS);
                break;
        }

        if (setsid() < 0) {
            fprintf(stderr, "%s: error in setsid(): %s\n", progname, strerror(errno));
            exit(EXIT_FAILURE);
        }

        //       switch (pid = fork()) {
        //           case -1: /* Fehler */
        //               syslog(LOG_ERR, "Fehler in fork(): %s.\n", strerror(errno));
        //               exit(EXIT_FAILURE);
        //               break;
        //           case 0: /* Child process continues */
        //               break;
        //           default: /* Parent process terminated immediately */
        //               exit(EXIT_SUCCESS);
        //               break;
        //       }
        //        close(STDIN_FILENO);
        //        close(STDOUT_FILENO);
        //        close(STDERR_FILENO);

        //        chdir("/");
        //        umask(0);
    } else {
        LOG__DEBUG("process continues direct, no daemon");
    }

    if (options.daemon) sleep(10);

    int serial_fd = openSerial(options.device, options.baud);
    if (serial_fd < 0) {
        perror("Serial open failed");
        return 1;
    }

    int udp_fd = setup_udp_client_socket(UDP_IP, UDP_PORT);
    if (udp_fd < 0) {
        perror("UDP socket failed");
        return 1;
    }

    uint8_t buffer[1024];
    mavlink_message_t msg;
    mavlink_status_t status;

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serial_fd, &readfds);
        FD_SET(udp_fd, &readfds);

        int maxfd = (serial_fd > udp_fd) ? serial_fd : udp_fd;
        struct timeval timeout = {1, 0};

        int ret = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
        if (ret < 0) {
            perror("select");
            break;
        } else if (ret == 0) {
            continue;
        }
        
        if (FD_ISSET(serial_fd, &readfds)) {
            ssize_t len = readSerial(serial_fd, buffer, sizeof(buffer));
            if (len > 0) {
                send_udp_packet(udp_fd, buffer, len);
            }
        }

        if (FD_ISSET(udp_fd, &readfds)) {
            ssize_t len = recv_udp_packet(udp_fd, buffer, sizeof(buffer));
            if (len > 0) {
                writeSerial(serial_fd, buffer, len);
            }
        }
    }

    while (false) {
        int len = readSerial(serial_fd, buffer, sizeof (buffer));
        if (len <= 0) {
            usleep(1000);
            continue;
        }
        //printf("begin len%d\n", len);
        for (int i = 0; i < len; ++i) {
            if (mavlink_parse_char(MAVLINK_COMM_0, buffer[i], &msg, &status)) {

                if (msg.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
                    mavlink_heartbeat_t hb;
                    mavlink_msg_heartbeat_decode(&msg, &hb);
                    bool armed = hb.base_mode & MAV_MODE_FLAG_SAFETY_ARMED;
                    printf("ARMED STATUS: %s\n", armed ? "YES" : "NO");

                } else if (msg.msgid == MAVLINK_MSG_ID_SYS_STATUS) {
                    mavlink_sys_status_t sys;
                    mavlink_msg_sys_status_decode(&msg, &sys);

                    float voltage = sys.voltage_battery / 1000.0f; // mV → V
                    float current = sys.current_battery / 100.0f; // cA → A

                    printf("Batterie: %.2f V, %.2f A\n", voltage, current);

                } else if (msg.msgid == MAVLINK_MSG_ID_BATTERY_STATUS) { /* 147 */
                    mavlink_battery_status_t bat;
                    mavlink_msg_battery_status_decode(&msg, &bat);

                    /*printf("Battery ID: %d\n", bat.id);
                    for (int i = 0; i < bat.cell_count; ++i) {
                        if (bat.voltages[i] != UINT16_MAX)
                            printf("        Zelle %d: %.3f V\n", i + 1, bat.voltages[i] / 1000.0f);
                    }
                    printf("  Strom: %.2f A\n", bat.current_battery / 100.0f);*/

                } else if (msg.msgid == MAVLINK_MSG_ID_POWER_STATUS            /* 125 */ ||
                        msg.msgid == MAVLINK_MSG_ID_SERVO_OUTPUT_RAW           /*  36 */ ||
                        msg.msgid == MAVLINK_MSG_ID_SYSTEM_TIME                /*   2 */ ||
                        msg.msgid == MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT      /* 62 */ ||
                        msg.msgid == MAVLINK_MSG_ID_RC_CHANNELS                /* 65 */ ||
                        msg.msgid == MAVLINK_MSG_ID_RAW_IMU                    /* 27 */ ||
                        msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT                /* 24 */ ||
                        msg.msgid == MAVLINK_MSG_ID_SCALED_PRESSURE            /* 29 */ ||
                        msg.msgid == MAVLINK_MSG_ID_ATTITUDE                   /* 30 */ ||
                        msg.msgid == MAVLINK_MSG_ID_LOCAL_POSITION_NED         /* 32 */ ||
                        msg.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT        /* 33 */ ||
                        msg.msgid == MAVLINK_MSG_ID_MISSION_CURRENT            /* 42*/ ||
                        msg.msgid == MAVLINK_MSG_ID_VFR_HUD                    /* 74 */ ||
                        msg.msgid == MAVLINK_MSG_ID_POSITION_TARGET_GLOBAL_INT /* 87 */ ||
                        msg.msgid == MAVLINK_MSG_ID_TIMESYNC                   /* 111 */ ||
                        msg.msgid == MAVLINK_MSG_ID_TERRAIN_REQUEST            /* 133 */ ||
                        msg.msgid == MAVLINK_MSG_ID_TERRAIN_CHECK              /* 135 */ ||
                        msg.msgid == MAVLINK_MSG_ID_TERRAIN_REPORT             /* 136 */ ||
                        msg.msgid == MAVLINK_MSG_ID_VIBRATION                  /* 241 */
                        ) {

                    //printf("### finde diese msg-id %d\n", msg.msgid);

                } else {
                    printf("### unknown msg-id %d\n", msg.msgid);
                }

                uint8_t out_buf[MAVLINK_MAX_PACKET_LEN];
                int out_len = mavlink_msg_to_send_buffer(out_buf, &msg);
                send_udp_packet(udp_fd, out_buf, out_len);
            }
        }
        //printf("ende\n");

        uint8_t bufferRX[1024];
        int lenRX = recv_udp_packet(udp_fd, bufferRX, sizeof(bufferRX));
        //printf("empfange per udp %d bytes\n", lenRX);
        if (lenRX <= 0) {
            usleep(1000);
            continue;
        }
        for (int i = 0; i < lenRX; ++i) {
            mavlink_message_t msgRX;
            mavlink_status_t statusRX;
            if (mavlink_parse_char(MAVLINK_COMM_1, bufferRX[i], &msgRX, &statusRX)) {
                printf("[UDP] MAVLink msgid: %d\n", msgRX.msgid);
                // → Hier MAVLink von Mission Planner verarbeiten
                uint8_t out_buf[MAVLINK_MAX_PACKET_LEN];
                int out_len = mavlink_msg_to_send_buffer(out_buf, &msgRX);
                printf("upd-rx-> len:%d\n", out_len);
                writeSerial(serial_fd, out_buf, out_len);
            }
        }
        //writeSerial(serial_fd, bufferRX, sizeof(bufferRX));
        
    }

    closeSerial(serial_fd);
    close(udp_fd);
    return 0;
}
