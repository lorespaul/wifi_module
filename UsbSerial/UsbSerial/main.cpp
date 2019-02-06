//
//  main.cpp
//  UsbSerial
//
//  Created by Lorenzo Daneo on 04/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <iostream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

#define BUFFER_LENGTH 100
char readBuffer[BUFFER_LENGTH];
char* bufptr;

int fd;

int open_port(void)
{
    fd = open("/dev/cu.usbserial-1420", O_RDWR | O_NOCTTY);// | O_NDELAY);
    if (fd == -1)
    {
        perror("open_port: Unable to open /dev/cu.usbserial-1420");
    }
    
    fcntl(fd, F_SETFL, 0);
    
    struct termios options;
    tcgetattr(fd, &options);
    
    cfsetispeed(&options, 74880);
    cfsetospeed(&options, 74880);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(fd,TCSANOW,&options);
    
    return (fd);
}

int write(const char* buff){
//    int i = (int)write(fd, buff, strlen(buff));
    int i = (int)write(fd, "AT\r", 3);
    return i;
}

void read(){
//    size_t nbytes;
//    bufptr = readBuffer;
//    while ((nbytes = read(fd, bufptr, readBuffer + sizeof(readBuffer) - bufptr - 1)) > 0)
//    {
//        bufptr += nbytes;
//        if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
//            break;
//    }
//    *bufptr = '\0';
//    read(fd, readBuffer, 3);
    int numInBytes = -1;
    int total = 0;
    do {
        numInBytes = (int)read(fd, readBuffer, sizeof(readBuffer));
        total += numInBytes;
    } while (readBuffer[total-1] != '\0');
}

int main(int argc, const char * argv[]) {
    // insert code here...
    open_port();
    
    write("AT\r");
    read();
    printf("%s", readBuffer);
    
    close(fd);
    return 0;
}
