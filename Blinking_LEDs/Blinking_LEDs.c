/*
 Copyright (c) 2014 Intel Corporation.
*
* Author: Sulamita Garcia
* Based on LEDblink.c by Nandkishor Sonar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
 * The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

/* This code blinks LEDs connected to Galileo GPIO3 (Arduino PIN13), GPIO17,
 * GPIO24 and GPIO27 (pins 5 6 and 7)
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define GP_LED                 (3) // GPIO3 is GP LED - LED connected between Cypress CY8C9540A and RTC battery header
#define GP_5				   (17) //GPIO17 corresponds to Arduino PIN5
#define GP_6				   (24) //GPIO24 corresponds to Arduino PIN6
#define GP_7				   (27) //GPIO27 corresponds to Arduino PIN7
#define BLINK_TIME_SEC         (1) // 1 seconds blink time
#define GPIO_DIRECTION_IN      (1)
#define GPIO_DIRECTION_OUT     (0)
#define ERROR                  (-1)


int openGPIO(int gpio, int direction )
{
        char buffer[256];
        int fileHandle;
        int fileMode;

  //Export GPIO
        fileHandle = open("/sys/class/gpio/export", O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Error: Unable to opening /sys/class/gpio/export");
               return(-1);
        }
        sprintf(buffer, "%d", gpio);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);

   //Direction GPIO
        sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               return(-1);
        }

        if (direction == GPIO_DIRECTION_OUT)
        {
               // Set out direction
               write(fileHandle, "out", 3);
               fileMode = O_WRONLY;
        }
        else
        {
               // Set in direction
               write(fileHandle, "in", 2);
               fileMode = O_RDONLY;
        }
        close(fileHandle);


   //Open GPIO for Read / Write
        sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
        fileHandle = open(buffer, fileMode);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               return(-1);
        }

        return(fileHandle);  //This file handle will be used in read/write and close operations.
}

int writeGPIO(int fHandle, int val)
{
        if(val ==  0)
        {
               // Set GPIO low status
               write(fHandle, "0", 1);
        }
        else
        {
               // Set GPIO high status
               write(fHandle, "1", 1);
        }

        return(0);
}

int readGPIO(int fileHandle)
{
        int value;

        read(fileHandle, &value, 1);

        if('0' == value)
        {
             // Current GPIO status low
               value = 0;
        }
        else
        {
             // Current GPIO status high
               value = 1;
        }

        return value;
}

int closeGPIO(int gpio, int fileHandle)
{
        char buffer[256];

        close(fileHandle); //This is the file handle of opened GPIO for Read / Write earlier.

        fileHandle = open("/sys/class/gpio/unexport", O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               return(-1);
        }
        sprintf(buffer, "%d", gpio);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);

        return(0);
}


int main(void)
{
        int fileHandleGPIO_LED;
        int fileHandleGPIO_5;
        int fileHandleGPIO_6;
        int fileHandleGPIO_7;
        int i=0;

        puts("Starting LED blink GP_LED - gpio-3 on Galileo board.");

        fileHandleGPIO_LED = openGPIO(GP_LED, GPIO_DIRECTION_OUT);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_OUT);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_OUT);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_OUT);


        if(ERROR ==  fileHandleGPIO_LED)
        {
               return(-1);
        }


        for(i=0; i< 10; i++)
        {
               //LED ON
               writeGPIO(fileHandleGPIO_LED, 1);
               writeGPIO(fileHandleGPIO_5, 1);
               writeGPIO(fileHandleGPIO_6, 1);
               writeGPIO(fileHandleGPIO_7, 1);
               sleep(BLINK_TIME_SEC);

               //LED OFF
               writeGPIO(fileHandleGPIO_LED, 0);
               writeGPIO(fileHandleGPIO_5, 0);
               writeGPIO(fileHandleGPIO_6, 0);
               writeGPIO(fileHandleGPIO_7, 0);
               sleep(BLINK_TIME_SEC);
        }

        closeGPIO(GP_LED, fileHandleGPIO_LED);
        closeGPIO(GP_5, fileHandleGPIO_5);
        closeGPIO(GP_6, fileHandleGPIO_6);
        closeGPIO(GP_7, fileHandleGPIO_7);


        puts("Finished LED blink GP_LED - gpio-3 on Galileo board.");

        return 0;
}
