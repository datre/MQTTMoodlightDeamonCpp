/***************************************************************
 * Name:        Serial Class for Arduino
 * Purpose:     Code for Serial Communication between Arduino and a PC
 * Author:      (Daniel Treitinger, John Pye)
 * Created:     2015-04-06
 * Copyright:   (Daniel Treitinger, John Pye)
 * License:     GPLv3
 **************************************************************/

#include <iostream>
#ifndef SERIALPORT_H
#define SERIALPORT_H


#include <stdint.h>   /* Standard types */
#include <unistd.h>   /* UNIX standard function definitions */


#ifdef _WIN32
# include <windows.h>
# define SERIALPORT_TERMIOS DCB
# define SERIALPORT_HANDLE HANDLE
# define SERIALPORT_FILE_ERROR INVALID_HANDLE_VALUE
# define SERIALPORT_SLEEP(N) Sleep(N)
#else
# include <termios.h>
# define SERIALPORT_TERMIOS struct termios
# define SERIALPORT_HANDLE int
# define SERIALPORT_FILE_ERROR -1
# define SERIALPORT_SLEEP(N) usleep(N*1e3)
#endif

/**
    Open serial port for communication.
    @param serialport name of the port to be opened (eg /dev/tty.usbserial)
    @param baud baud rate (integer)
    @param oldsettings pointer to a struct in which old comm settings should be
    stored, if desired (else NULL if not desired)

    @return valid file descriptor, or -1 on error.
*/
using namespace std;
class Serial
{
public:
    Serial(string serialport, int baud);
    ~Serial();
    SERIALPORT_HANDLE serialport_init(const char* serialport, int baud);
    int serialport_writebyte(uint8_t b);
    /*
        Read a byte from the serial connection.
        @return 0 on success
    */
    int serialport_readbyte(char *byte);
    int serialport_write(const char* str, int length);
    int serialport_read_until(char* buf, char until, int timeout = 5000);
    //int serialport_restoresettings(SERIALPORT_TERMIOS *oldsettings);
    /**
        Close a success-opened connection, return 0 on success.
    */
    int serialport_close();
    SERIALPORT_HANDLE Getfd(){return fd;};

    private:
        SERIALPORT_HANDLE fd;
        //SERIALPORT_TERMIOS *oldsettings = NULL;
        //char* port;

};


#endif
