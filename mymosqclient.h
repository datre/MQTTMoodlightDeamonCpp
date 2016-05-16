#ifndef MYMOSQCLIENT_H
#define MYMOSQCLIENT_H
#include <mosquittopp.h>
#include <string.h>
#include <string>
#include <iostream>
#include <syslog.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <vector>
#include "Serial.h"
//#define DEAMON
#define PORT "/dev/ttyUSB0"
#define BAUDRATE 115200
#define HOST "localhost"
#define TOPIC  "kitchen/switches/#"
#define qos 1



class MQTTSerialInterface : public mosqpp::mosquittopp
{
public:
    MQTTSerialInterface(const char* id, const char* host = "localhost", int port = 1883, int keepalive = 60);
    ~MQTTSerialInterface();
private:
    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_error();
    //void on_exit();
    void on_log(int level, const char *msg);
    void on_publish(int mid);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);
    void on_unsubscribe(int mid);
    void log(std::stringstream &info, int prio = LOG_ALERT);
    void setcolor(int color, int lightn);
    void evaluateMQTT(std::string payload, std::string topic);
    char Crc8(const char *data, int len);
    Serial *serialPort;
    char *serialData;
};

#endif // MYMOSQCLIENT_H
