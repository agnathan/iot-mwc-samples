/*
 Copyright (c) 2014 Intel Corporation  All Rights Reserved.
 The source code, information and material ("Material") contained herein is owned by Intel Corporation or its suppliers or licensors, and title to such Material remains with Intel Corporation 
 or its suppliers or licensors. The Material contains proprietary information of Intel or its suppliers and licensors. The Material is protected by worldwide copyright laws and treaty provisions. 
 No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted, transmitted, distributed or disclosed in any way without Intel's prior express written permission. 
 No license under any patent, copyright or other intellectual property rights in the Material is granted to or conferred upon you, either expressly, by implication, inducement, estoppel or otherwise. 
 Any license under such intellectual property rights must be express and approved by Intel in writing.

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

Include supplier trademarks or logos as supplier requires Intel to use, preceded by an asterisk. An asterisked footnote can be added as follows: *Third Party trademarks are the property of their respective owners.

Unless otherwise agreed by Intel in writing, you may not remove or alter this notice or any other notice embedded in Materials by Intel or Intel’s suppliers or licensors in any way.”

*/




#include <stdlib.h>

#include <stdio.h>

#include <fcntl.h>

#include <unistd.h>

#include <string.h>



#define GP_LED                 (39) // GPI13 is GP LED

#define GP_PROXY               (26) // GPI08 is PROXIMITY INPUT

#define GP_PWM               	(3) // PWM3  is PWM output

#define GPIO_DIRECTION_IN      (1)

#define GPIO_DIRECTION_OUT     (0)

#define ERROR                  (-1)


int flag = 1;
int duty_cycle = 0;

int openFileForReading(gpio)
{
    char buffer[256];
    int fileHandle;

    sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);

    fileHandle = open(buffer, O_RDONLY);
    if(ERROR == fileHandle)
    {
	   puts("Unable to open file:");
	   puts(buffer);
	   return(-1);
    }
    return(fileHandle);  //This file handle will be used in read/write and close operations.

}

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

int readGPIO(int fileHandle,int gpio)

{
        int value;
        //Reopening the file again in read mode, since data was not refreshing.
        fileHandle = openFileForReading(gpio);
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
        close(fileHandle);
        return value;
}


int closePWM(int pwm)

{
        char buffer[256];
        int fileHandle;

        fileHandle = open("/sys/class/gpio/unexport", O_WRONLY);
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
        int fileHandleGPIO_LED;
        int fileHandleGPIO_PROXY;
        int i=0;

        puts("Starting proximity reader on Galileo board.");
        fileHandleGPIO_PROXY = openGPIO(GP_PROXY, GPIO_DIRECTION_IN);
        if(ERROR ==  fileHandleGPIO_PROXY)
        {
        		puts("Unable to open toggle Proximity port #8");
        		return(-1);
        }
        fileHandleGPIO_LED = openGPIO(GP_LED, GPIO_DIRECTION_OUT);

        if(ERROR ==  fileHandleGPIO_LED)

        {
        		puts("Unable to open toggle LED port #13");
        		return(-1);

        }


        //Switch off the LED before starting.
        writeGPIO(fileHandleGPIO_LED, 0);

        //set PWM parameters
        openPWM(GP_PWM);
        setPWMPeriod(1000000,GP_PWM);
        enablePWM(1,GP_PWM);
		setPWMDutyCycle(0,GP_PWM);


        //Start an infinite loop to keep polling for proximity info
        int proxyValue = 0;
        while(1==1)
        {
			proxyValue = readGPIO(fileHandleGPIO_PROXY,GP_PROXY);
			if(proxyValue == 1)
			{
				if(duty_cycle == 500000)
				{
					duty_cycle = 200000;
		        	writeGPIO(fileHandleGPIO_LED, 0);
				}
				else
				{
					duty_cycle = 500000;
		        	writeGPIO(fileHandleGPIO_LED, 1);
				}
				setPWMDutyCycle(duty_cycle,GP_PWM);
			}
			else
			{
				duty_cycle = 50000;
				setPWMDutyCycle(0,GP_PWM);
	        	writeGPIO(fileHandleGPIO_LED, 0);
			}
        	usleep(1000*400);
        }


        closeGPIO(GP_LED, fileHandleGPIO_LED);
        closeGPIO(GP_PROXY, fileHandleGPIO_PROXY);
        closePWM(GP_PWM);

        puts("Finished BURGLER ALARM on Galileo board.");
        return 0;
}
