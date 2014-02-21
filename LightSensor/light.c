/*
Copyright (c) 2014 Intel Corporation.
*
* Author: Raghavendra Ural
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

/* This code use
This is an analog input example. This sample uses light sensor.
Functionality:
this application reads analog pin A0 and modulates PWM4 (PIN 11 on board) based on the amount of light.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define GP_ANALOG_IN            (37) // A0
#define GP_PWM                    (4) // // PWM output. PIN 11 on the board
#define GPIO_DIRECTION_OUT        (0)
#define GPIO_DIRECTION_IN         (1)
#define ERROR                  (-1)


int flag = 1;
int duty_cycle = 0;

int openPWM(int port)
{
        char buffer[256];
        int fileHandle;
        int fileMode;

        //Export GPIO
        fileHandle = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);

        if(ERROR == fileHandle)

        {
               puts("Error: Unable to opening /sys/class/gpio/export");
               return(-1);
        }
        sprintf(buffer, "%d", port);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);
        sleep(1);
        return 0;
}

int enablePWM(int enable,int port)
{
              char buffer[256];

              int fileHandle;


              //Enable PWM
        sprintf(buffer, "/sys/class/pwm/pwmchip0/pwm%d/enable", port);
        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");

               puts(buffer);

               return(-1);

        }

              sprintf(buffer, "%d", enable);
        write(fileHandle, buffer, strlen(buffer));
        return 0;
}

int setPWMPeriod(int period, int port)
{
       //Open GPIO for Read / Write
              char buffer[256];
              int fileHandle;


              sprintf(buffer, "/sys/class/pwm/pwmchip0/pwm%d/period", port);

        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");

               puts(buffer);

               return(-1);
        }
              sprintf(buffer, "%d", period);
        write(fileHandle, buffer, strlen(buffer));

        close(fileHandle);
        return(0);

}

int setPWMDutyCycle(int dutycycle, int port)
{
       //Open GPIO for Read / Write
              char buffer[256];
              int fileHandle;


              sprintf(buffer, "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", port);

        fileHandle = open(buffer, O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");

               puts(buffer);

               return(-1);
        }
              sprintf(buffer, "%d", dutycycle);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);
        return(0);

}


int openGPIO(int gpio, int direction,int gpioValue)
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

        if(gpioValue ==  0)

        {
               // Set GPIO low status
               write(fileHandle, "0", 1);
        }
        else

        {
               // Set GPIO high status
               write(fileHandle, "1", 1);

        }

        return(fileHandle);  //This file handle will be used in read/write and close operations.

}


int readAnalogIn(int port)

{
       ///sys/bus/iio/devices/iio\:device0/in_voltage0_raw
        int value;
        char buffer[256];
        FILE *fileHandle;

        sprintf(buffer, "/sys/bus/iio/devices/iio\:device0/in_voltage%d_raw", port);

        fileHandle = fopen(buffer, "r+");
        if(NULL == fileHandle)
        {
          puts("Unable to open file:");
          puts(buffer);
          return(-1);
        }
        fscanf(fileHandle, "%d", &value);
        fclose(fileHandle);  //This file handle will be used in read/write and close operations.
        return value;
}


int closePWM(int pwm)

{
        char buffer[256];
        int fileHandle;

        fileHandle = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               return(-1);
        }
        sprintf(buffer, "%d", pwm);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);
        return(0);
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
        int fileHandleGPIO_ANALOG;

        puts("Starting light sensor reader on Galileo board.");
        fileHandleGPIO_ANALOG = openGPIO(GP_ANALOG_IN, GPIO_DIRECTION_OUT,0);
        if(ERROR ==  fileHandleGPIO_ANALOG)
        {
                     puts("Unable to open toggle Proximity port #8");
                     return(-1);
        }
        //set PWM parameters
        openPWM(GP_PWM);
        setPWMPeriod(1000000,GP_PWM);
        enablePWM(1,GP_PWM);
              setPWMDutyCycle(0,GP_PWM);


        //Start an infinite loop to keep polling for proximity info
        int analogValue = 0;
        while(1==1)
        {
                     analogValue = readAnalogIn(0);
                     if(analogValue <= 100)
                     {
                           setPWMDutyCycle(800000,GP_PWM);
                     }
                     else if(analogValue > 100 && analogValue < 500)
                     {
                           setPWMDutyCycle(300000,GP_PWM);
                     }
                     else if(analogValue >= 500 && analogValue < 1000)
                     {
                           setPWMDutyCycle(100000,GP_PWM);
                     }
                     else
                     {
                           setPWMDutyCycle(0,GP_PWM);
                     }
              usleep(1000*400);
        }


        closeGPIO(GP_ANALOG_IN,fileHandleGPIO_ANALOG);
        closePWM(GP_PWM);

        puts("Finished Light Sensor reader on Galileo board.");
        return 0;
}
