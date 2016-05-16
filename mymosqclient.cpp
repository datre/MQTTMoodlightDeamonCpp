#include "mymosqclient.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

MQTTSerialInterface::MQTTSerialInterface(const char* id, const char* host, int port, int keepalive) : mosquittopp(id)
{
#ifdef DEAMON
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(id, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#endif
    mosqpp::lib_init();
    connect(host, port, keepalive);
    loop_start();
    serialPort = new Serial(PORT, BAUDRATE);
    serialData = new char[35];
    for(int i=0; i<35; i++)
    {
        serialData[i] = 0x00;
    }
    //The things that are always the same
    serialData[0] = 0x40; //that what is called "preamble" in Go Code
    serialData[1] = 0xFE; //Source/Clientaddress in Go Code
    serialData[2] = 0x10; //Controlleradress
    serialData[3] = 0x1E; //Lenght of the Payload
}

MQTTSerialInterface::~MQTTSerialInterface()
{
    loop_stop();
    mosqpp::lib_cleanup();
#ifdef DEAMON
    closelog();
#endif
}

void MQTTSerialInterface::on_connect(int rc)
{
    std::stringstream logmsg;
    logmsg << "Connected, rc: " << rc << "\n";
    log(logmsg);
}

void MQTTSerialInterface::on_disconnect(int rc)
{
    std::stringstream logmsg;
    logmsg << "Disconnected, rc: " << rc << "\n";
    log(logmsg);
}

void MQTTSerialInterface::on_error()
{
    std::stringstream logmsg;
    logmsg << "Error\n";
    log(logmsg);
}

void MQTTSerialInterface::on_log(int level, const char *msg)
{
    //syslog(LOG_ALERT, msg);
    std::stringstream logmsg;
    logmsg << "Log Level " << level << " Message: " << msg << "\n";
    log(logmsg);
}

void MQTTSerialInterface::on_message(const mosquitto_message *message)
{
    std::stringstream logmsg;
    char* payload = new char[message->payloadlen + 1];
    logmsg << "Topic: " << message->topic << "\n";
    strncpy(payload, (char*) message->payload, message->payloadlen); //to prohibit strange letters
    payload[message->payloadlen] = '\0';
    logmsg << "Payload/Message: " << payload << "\n";
    log(logmsg);
    evaluateMQTT(std::string(payload), std::string(message->topic));
}

void MQTTSerialInterface::on_publish(int mid)
{
    //Not Used
}

void MQTTSerialInterface::on_subscribe(int mid, int qos_count, const int *qos_granted)
{
  //  syslog(LOG_ALERT, "Info New");
    std::stringstream logmsg;
    logmsg << "Subscribed\nGranted qos: " << *qos_granted << "\nQos count: " << qos_count << "\nMessage ID:" << mid << "\n";
    log(logmsg);
}

void MQTTSerialInterface::on_unsubscribe(int mid)
{
    std::stringstream logmsg;
    logmsg << "Unsubscribed\nMessage ID:" << mid << "\n";
    log(logmsg);
}

void MQTTSerialInterface::log(std::stringstream &info, int prio)
{
#ifdef DEAMON
    syslog(prio , info.str().c_str());
#else
    std::cout << info.str() << std::endl;
#endif

}

void MQTTSerialInterface::setcolor(int color, int lightn)
{
    if(lightn<=10 && lightn>0)
    {
        for(int i = 0; i<3; i++)
        {
            //Offset of 4 because color begin after preamble, Clientaddress, Controlleraddress and Payloadlenght, lightn-1 because n is not a zero based counter it is a 1 based counter
            // +i : When i is 0 it addresses the red byte, when i is 1 addresses the green byte and when i is 2 it addresses the blue byte TODO: Check if the color position is correct
            *(serialData + 4 + 3*(lightn-1) + i) = (char) (color>>((2-i)*2)) & 0xFF; // TODO Clear if i-2 or i only
            // example color int: 00 f8 34 23 (with i == 1)
            // after shift int: 00 00 f8 34
            // remove the rest with and 0xFF: 00 00 00 34
            // typecast to char to prevent writing on other bytes: 34
            char buf[30];
            strncpy(buf, serialData+4, 30);
            serialData[34] = Crc8(buf, 30);
        }
    }
}

char MQTTSerialInterface::Crc8(const char *data, int len)//TODO Check if that works correct
{
    unsigned crc = 0;
    int i, j;
    for (j = len; j; j--, data++)
    {
        crc ^= (*data << 8);
        for(i = 8; i; i--)
        {
            if (crc & 0x8000)
            {
                crc ^= (0x1070 << 3);
            }
            crc <<= 1;
        }
    }
    return (char)(crc >> 8);
}

void MQTTSerialInterface::evaluateMQTT(std::string payload, std::string topic)
{
    if(topic == "/kitchen/shutdown/")
    {
        for(int i=4; i<35; i++)
        {
            serialData[i] = 0;
        }

    }
    else
    {
        int color, lampnb;
        std::stringstream colorcode, lampnumber;
        std::vector<std::string> temp = split(payload, '#'); //example Payload: 2#ff88aa

        lampnumber << temp[0];
        lampnumber >> lampnb;
        colorcode << std::hex << temp[1]; //TODO: Check if temp[1] can be converted to std::hex;
        colorcode >> color;
        setcolor(color, lampnb);
    }
    if(serialPort->Getfd() != -1)
    {
        serialPort->serialport_write(serialData, 35);
    }
}
