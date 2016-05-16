#include <iostream>
#include <string.h>
#include "mymosqclient.h"
//#define DEAMON 1

using namespace std;
int keepalive = 120;
int port = 1883;
char host[] = "localhost";
char topic[] = "kitchen/switches/#";
char deamon_name_and_id[] = "MQTTMoodlightDeamon";
int qos = 1;

int main()
{
#ifdef DEAMON
    //syslog(LOG_INFO, "Entering Daemon");
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) { exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }
    umask(0);
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif // DEAMON
    MQTTSerialInterface *client = new MQTTSerialInterface(deamon_name_and_id, host, port, keepalive);
    client->subscribe(NULL, topic, qos);
    while(true)
    {
        usleep(1000000);
    }

    return 0;
}

