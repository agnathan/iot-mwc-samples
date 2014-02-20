
#include "parson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <curl/curl.h>

#define GP_IO4 		(28) /* GPIO4 port */
#define GP_IO5 		(17) /* GPIO5 port */
#define GP_IO6 		(24) /* GPIO6 port */
#define GP_IO7 		(27) /* GPIO7 port */
#define GP_IO8 		(26) /* GPIO8 port */

#define GPIO_ON  	(1)
#define GPIO_OFF 	(0)
#define ERROR		(-1)

/* methods to retrieve and parse the weather information */
int getWeatherInfo(void);
int parseWeatherInfo(void);
int updateWeatherData(int);

/* methods to write the ON/OFF values to the GPIO port files */
int writeToGPIOPort(int, int);
int openGPIOFile(int, int);

static char output_filename[] = "weather-info.json";

typedef enum {SUNNY = 113, CLOUDY = 122, DRIZZLE = 263, SHOWERS = 308, FOG = 260, SNOW = 227, LIGHT_SLEET = 311, HEAVY_SLEET = 320};

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}


int main() {

	int retValue;
    retValue = getWeatherInfo();

    if(retValue != 0)
	{
		return(-1);
	}
    retValue = parseWeatherInfo();
	//retValue = updateWeatherData(LIGHT_SLEET);
	if(retValue != 0)
	{
		return(-1);
	}
    return 0;
}


int getWeatherInfo(void)
{
  CURL *curl_handle;
  FILE * fptr; 
  /* URL for the API call */
  char *url="http://api.worldweatheronline.com/free/v1/weather.ashx?q=Barcelona&format=json&num_of_days=1&key=y3rhascwewjdtearvnujbymk";
  CURLcode res;
  
  curl_handle = curl_easy_init();
  if(curl_handle) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);	

     /* send all data to this function  */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* Now open the file */
    fptr = fopen(output_filename,"w+");
    if(!fptr) {
       printf("Exiting, error opening file");
      /* always cleanup */
      curl_easy_cleanup(curl_handle);
      return 1;
    }
   /* write content to the file handle */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fptr); 

    res = curl_easy_perform(curl_handle);
    printf("\nWeather Report Query Status:: %d\n", res);

    /* Close the file and cleanup */
    fclose (fptr); 
    curl_easy_cleanup(curl_handle);
	
  }
  return 0;
}


int parseWeatherInfo(void) {
    JSON_Value *root_value;
    JSON_Array *array;
    JSON_Object *root_object, *CCobject;
    const char *weatherValue;
    int ret_val = -1; 
    
    char cleanup_file[256];

    sprintf(cleanup_file, "rm -f %s", output_filename);
    
    /* now parse the json file */

    root_value = json_parse_file(output_filename);
    if (json_value_get_type(root_value) != JSONObject) {
        return 0;
    }    
    /* get the root object */
    root_object = json_value_get_object(root_value); 
    
    /* Now get the required array from the root object*/
    array = json_object_dotget_array(root_object, "data.current_condition");
    if (array != NULL) {

        /* The array has target objects, so get the object of first array element*/
        CCobject = json_array_get_object(array,0);

        /* Now with the target object retrieve the values */
        // weatherValue = json_object_get_string(CCobject, "weatherCode");
        /* typedef enum {
         * SUNNY = 113, CLOUDY = 122,
         * DRIZZLE = 263, SHOWERS = 308,
         *  FOG = 260, SNOW = 227,
         *  LIGHT_SLEET = 311, HEAVY_SLEET = 320};
         */

        printf("Current WeatherCode: %s\n", weatherValue );
	ret_val = updateWeatherData(atoi(weatherValue));
	printf("Return Value = %d\n", ret_val);
     } else {
       printf("Empty array\n");
     }

    /* cleanup code */
    json_value_free(root_value);
    system(cleanup_file);
    return 0;
}

int updateWeatherData(int val)
{
	int retValue;

    writeToGPIOPort(GP_IO4, GPIO_OFF);
    writeToGPIOPort(GP_IO5, GPIO_OFF);
    writeToGPIOPort(GP_IO6, GPIO_OFF);
    writeToGPIOPort(GP_IO7, GPIO_OFF);
    writeToGPIOPort(GP_IO8, GPIO_OFF);

  	if(SUNNY == val){
		retValue = writeToGPIOPort(GP_IO4, GPIO_ON);  /* sunny weather */
                printf("Sunny\n ");
    }
    else if(SUNNY < val && CLOUDY >= val){
		retValue = writeToGPIOPort(GP_IO5, GPIO_ON);   /* cloudy weather */
 printf("cloudy \n");
    }
    else if(DRIZZLE <= val && SHOWERS >= val){
 		retValue = writeToGPIOPort(GP_IO6, GPIO_ON);  /* Rainy weather */
 printf("Rainy\n");
    }
    else if(SNOW <= val && FOG >= val){
		retValue = writeToGPIOPort(GP_IO7, GPIO_ON); /* Snowy weather */
 printf("Snowy\n");
    }    
    else if(LIGHT_SLEET <= val && HEAVY_SLEET >= val){
		retValue = writeToGPIOPort(GP_IO8, GPIO_ON); /* Fleet/Extreme weather */
 printf("Extreme\n");
    }
    else{
		retValue = writeToGPIOPort(GP_IO8, GPIO_ON);  /* Fleet/Extreme weather */
 printf("Extreme\n");
    }
    return retValue;
}


int writeToGPIOPort(int gpioPort, int val)
{
	int gpioFileHandle;
	/* open the GPIO file */
	gpioFileHandle = openGPIOFile(gpioPort, val);	
	if(ERROR ==  gpioFileHandle)
	{
		return(-1);
	}
	if(val ==  0)
	{
		/* Set LOW on GPIO port */
		write(gpioFileHandle, "0", 1);
	}
	else
	{
		/* Set HIGH on GPIO port */
		write(gpioFileHandle, "1", 1);
	}
	close(gpioFileHandle);
	return(0);
}

int openGPIOFile(int gpio, int direction )
{
	char buffer[256];
	int fileHandle;
	int fileMode;

  /* Export GPIO */
	fileHandle = open("/sys/class/gpio/export", O_WRONLY);
	if(ERROR == fileHandle)
	{
		puts("Error: Unable to opening /sys/class/gpio/export");
		return(-1);
	}
	sprintf(buffer, "%d", gpio);
	write(fileHandle, buffer, strlen(buffer));
	close(fileHandle);

   /* Direction GPIO */
	sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
	fileHandle = open(buffer, O_WRONLY);
//	if(ERROR == fileHandle)
//	{
//		puts("Unable to open file:");
//		puts(buffer);
//		return(-1);
//	}
//
//	if (direction == GPIO_OFF)
//	{
		/* Set out direction */
		write(fileHandle, "out", 3);
		fileMode = O_WRONLY;
//	}
//	else
//	{
//		/* Set in direction */
//		write(fileHandle, "in", 2);
//		fileMode = O_RDONLY;
//	}
	close(fileHandle);


   /* Open GPIO for Read / Write */ 
	sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
	fileHandle = open(buffer, fileMode);
	if(ERROR == fileHandle)
	{
		puts("Unable to open file:");
		puts(buffer);
		return(-1);
	}

	return(fileHandle);  /* This file handle will be used in read/write and close operations. */
}

