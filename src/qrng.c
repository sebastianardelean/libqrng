/****************************************************************************
 * libqrng - library for interacting with IDQ's Quantis Appliance           *
 *                                                                          *
 * Copyright (C) 2023  Sebastian Mihai Ardelean                             *
 *                                                                          *
 * This program is free software: you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ****************************************************************************/

/**
 * @file qrng.c
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 24 May 2023
 * @brief API for interacting with IDQ's Quantis Appliance
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "qrng.h"


#define DOMAIN_ADDRESS_LENGTH 254u
#define URL_MAX_LENGTH 512u
#define DEFAULT_NUMBER_OF_SAMPLES 1u
#define MIN_VALUE_INT 0u
#define MAX_VALUE_INT 1u

#define MIN_VALUE_FLOAT 0.0f
#define MAX_VALUE_FLOAT 1.0f


typedef enum {
  BYTES_RANDOM_NUMBER = 0,
  INT16_RANDOM_NUMBER,
  INT32_RANDOM_NUMBER,
  DOUBLE_RANDOM_NUMBER,
  FLOAT_RANDOM_NUMBER,
  STREAM_BINARY,
  PERFORMANCE_REQUEST,
  FIRMWARE_INFO_REQUEST,
  SYSTEM_INFO_REQUEST,
  NUMBER_OF_REQUESTS
}e_req_type_t;


typedef struct
{
  e_req_type_t type;
  const char *api_url;
  char domain_address[DOMAIN_ADDRESS_LENGTH];
  size_t samples;
  int32_t min_range_i;
  int32_t max_range_i;
  double min_range_f;
  double max_range_f;
}s_api_t;

#ifdef NO_DYNAMIC_MEMORY_ALLOCATION
#define TOTAL_MAX_SIZE_RESPONSE CURL_MAX_HTTP_HEADER
typedef struct {
  char memory[TOTAL_MAX_SIZE_RESPONSE];
  size_t size;
}memory_t;
#else
typedef struct {
    char *memory;
    size_t size;
}memory_t;
#endif

static s_api_t api_types[] = {
  {
    .type = BYTES_RANDOM_NUMBER,
    .api_url = "https://%s/api/2.0/hexbytes?quantity=%lu&dataLength=1",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = INT16_RANDOM_NUMBER,
    .api_url = "https://%s/api/2.0/short?min=%d&max=%d&quantity=%lu",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = INT32_RANDOM_NUMBER,
    .api_url = "https://%s/api/2.0/int?min=%d&max=%d&quantity=%lu",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = DOUBLE_RANDOM_NUMBER,
    .api_url = "https://%s/api/2.0/double?min=%lf&max=%lf&quantity=%lu",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = FLOAT_RANDOM_NUMBER,
    .api_url = "https://%s/api/2.0/double?min=%lf&max=%lf&quantity=%lu",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = STREAM_BINARY,
    .api_url = "https://%s/api/2.0/streambytes?size=%lu",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  }, 
  {
    .type = PERFORMANCE_REQUEST,
    .api_url = "",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = FIRMWARE_INFO_REQUEST,
    .api_url = "https://%s/api/2.0/firmwareinfo",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  },
  {
    .type = SYSTEM_INFO_REQUEST,
    .api_url = "https://%s/api/2.0/systeminfo",
    .domain_address = "",
    .samples = DEFAULT_NUMBER_OF_SAMPLES,
    .min_range_i = MIN_VALUE_INT,
    .max_range_i = MAX_VALUE_INT,
    .min_range_f = MIN_VALUE_FLOAT,
    .max_range_f = MAX_VALUE_FLOAT
  }
};




static CURL *p_curl_handle = NULL;

static size_t curl_write_cbk(void *content, 
		      size_t size, 
		      size_t nmemb, 
		      void *userp);
static void create_req_url(e_req_type_t req_type, char *api_url);
static int execute_request(char *url, void *buffer);
static int execute_stream_request(char *url, void *buffer);

static void parse_response_string(char *random_values_string, void *buffer, size_t samples, e_req_type_t request_type);

int qrng_open(const char *device_domain_address){

    int retval = 0;
    size_t i = 0;
    if (device_domain_address[0] != '\0') {
      for (i = 0 ; i < NUMBER_OF_REQUESTS; i++) {
        strncpy(api_types[i].domain_address, device_domain_address, DOMAIN_ADDRESS_LENGTH);
      }

      
      if(curl_global_init(CURL_GLOBAL_ALL)!=0) {
	fprintf(stderr, "Error in curl_global_init");
	retval = -1;
	
      }
      else {
	p_curl_handle = curl_easy_init();
	if (!p_curl_handle) {
          fprintf(stderr, "Error in curl_easy_init");
          curl_global_cleanup();
          retval = -2;
	}
        else {

#ifdef DEBUG
          (void)curl_easy_setopt(p_curl_handle, CURLOPT_VERBOSE, 1L);
#endif
          (void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

          (void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	    	    
          (void)curl_easy_setopt(p_curl_handle, CURLOPT_FAILONERROR, 1L);
	}
      }
    }
    else {
      retval = -3;
    }
    return retval;
 
}


void qrng_close(void)
{
    if (p_curl_handle) {
	curl_easy_cleanup(p_curl_handle);
        curl_global_cleanup();
    }

}


int qrng_random_stream(FILE *stream, size_t size)
{
  int retval = 0;
  char final_url[URL_MAX_LENGTH] = {0};
  api_types[STREAM_BINARY].samples = size;
  create_req_url(STREAM_BINARY, final_url);
  retval = execute_stream_request(final_url, (void *)stream);
  return retval;
}


int qrng_random_double(double min, double max, size_t samples, double *buffer)
{
    int retval = 0;

    char final_url[URL_MAX_LENGTH]={0};

    
    memory_t mem_buffer;
    
    api_types[DOUBLE_RANDOM_NUMBER].samples = samples;
    api_types[DOUBLE_RANDOM_NUMBER].min_range_f = min;
    api_types[DOUBLE_RANDOM_NUMBER].max_range_f = max;
    
    memset(&mem_buffer, 0, sizeof(mem_buffer));

    create_req_url(DOUBLE_RANDOM_NUMBER, final_url);


    retval = execute_request(final_url, (void *)&mem_buffer);
      
    if (!retval) {
      /* parse values array */
      char *random_values_string = mem_buffer.memory;
      parse_response_string(random_values_string, buffer, samples, DOUBLE_RANDOM_NUMBER);
    }
    else {
      fprintf(stderr, "could not execute curl request");
    }
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION    
    if (mem_buffer.memory) {
      free(mem_buffer.memory);
    }
#endif
    return retval;
}

int qrng_random_float(float min, float max, size_t samples, float *buffer)
{
    int retval = 0;

    char final_url[URL_MAX_LENGTH]={0};

    
    memory_t mem_buffer;

    api_types[FLOAT_RANDOM_NUMBER].samples = samples;
    api_types[FLOAT_RANDOM_NUMBER].min_range_f = min;
    api_types[FLOAT_RANDOM_NUMBER].max_range_f = max;
    
    memset(&mem_buffer, 0, sizeof(mem_buffer));

    create_req_url(FLOAT_RANDOM_NUMBER, final_url);


    retval = execute_request(final_url, (void *)&mem_buffer);
      
    if (!retval) {
      /* parse values array */
      char *random_values_string = mem_buffer.memory;
      parse_response_string(random_values_string, buffer, samples, FLOAT_RANDOM_NUMBER);

    }
    else {
      fprintf(stderr, "could not execute curl request");
    }
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION    
    if (mem_buffer.memory) {
      free(mem_buffer.memory);
    }
#endif
    return retval;
}


int qrng_random_bytes(size_t samples, uint8_t *buffer)
{
    int retval = 0;
    char final_url[URL_MAX_LENGTH] = {0};
    
    memory_t mem_buffer;
    api_types[BYTES_RANDOM_NUMBER].samples = samples;
    
    memset(&mem_buffer, 0, sizeof(mem_buffer));
    create_req_url(BYTES_RANDOM_NUMBER, final_url);
    
    retval = execute_request(final_url, (void *)&mem_buffer);

    if (!retval) {
      /* parse values array */
      char *random_values_string = mem_buffer.memory;
      parse_response_string(random_values_string, buffer, samples, BYTES_RANDOM_NUMBER);
    }
    else {
      fprintf(stderr, "could not execute curl request");
    }
    
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION        
    if (mem_buffer.memory) {
      free(mem_buffer.memory);
    }
#endif
    return retval;
}


int qrng_random_int16(int16_t min, int16_t max, size_t samples, int16_t *buffer)
{
    int retval = 0;

    char final_url[URL_MAX_LENGTH]={0};

    
    memory_t mem_buffer;

    api_types[INT16_RANDOM_NUMBER].samples = samples;
    api_types[INT16_RANDOM_NUMBER].min_range_i = min;
    api_types[INT16_RANDOM_NUMBER].max_range_i = max;

    memset(&mem_buffer, 0, sizeof(mem_buffer));

    create_req_url(INT16_RANDOM_NUMBER, final_url);


    retval = execute_request(final_url, (void *)&mem_buffer);
      
    if (!retval) {
      /* parse values array */
      char *random_values_string = mem_buffer.memory;
      parse_response_string(random_values_string, buffer, samples, INT16_RANDOM_NUMBER);
    }
    else {
      fprintf(stderr, "could not execute curl request");
    }
    
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION        
    if (mem_buffer.memory) {
free(mem_buffer.memory);
    }
#endif
    return retval;
}

int qrng_random_int32(int32_t min, int32_t max, size_t samples, int32_t *buffer)
{
    int retval = 0;

    char final_url[URL_MAX_LENGTH]={0};

    
    memory_t mem_buffer;


    api_types[INT32_RANDOM_NUMBER].samples = samples;
    api_types[INT32_RANDOM_NUMBER].min_range_i = min;
    api_types[INT32_RANDOM_NUMBER].max_range_i = max;

    memset(&mem_buffer, 0, sizeof(mem_buffer));

    create_req_url(INT32_RANDOM_NUMBER, final_url);


    retval = execute_request(final_url, (void *)&mem_buffer);
      
    if (!retval) {
      /* parse values array */
      char *random_values_string = mem_buffer.memory;
      parse_response_string(random_values_string, buffer, samples, INT32_RANDOM_NUMBER);
    }
    else {
      fprintf(stderr, "could not execute curl request");
    }
#ifndef NO_DYNAMIC_MEMORY_ALLOCATION        
    if (mem_buffer.memory) {
      free(mem_buffer.memory);
    }
#endif   
    return retval;
}


int qrng_firmware_info(void *buffer) {
  int retval = 0;
  char final_url[URL_MAX_LENGTH] = {0};
  create_req_url(FIRMWARE_INFO_REQUEST, final_url);
  retval = execute_stream_request(final_url, (void *)buffer);
  return retval;
}

int qrng_system_info(void *buffer) {
  int retval = 0;
  char final_url[URL_MAX_LENGTH] = {0};
  create_req_url(SYSTEM_INFO_REQUEST, final_url);
  retval = execute_stream_request(final_url, (void *)buffer);
  return retval;
}


int execute_request(char *url, void *buffer) {
  CURLcode error = CURLE_OK;

  int retval = 0;



  (void)curl_easy_setopt(p_curl_handle, CURLOPT_URL, url);
  (void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEFUNCTION, &curl_write_cbk); \
  (void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEDATA, buffer);
  error = curl_easy_perform(p_curl_handle);
  
  if(error != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(error));
    retval = -1;
  }
  else {
    //do nothing
  }
  
  return retval;
}


int execute_stream_request(char *url, void *buffer) {
    CURLcode error = CURLE_OK;
    int retval = 0;

    (void)curl_easy_setopt(p_curl_handle, CURLOPT_URL, url);
    (void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEDATA, buffer);

    error = curl_easy_perform(p_curl_handle);
    if(error != CURLE_OK) {
	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(error));
        retval = -1;
    }
    else {
	//do nothing
    }
  
    return retval;
}


void create_req_url(e_req_type_t req_type, char *api_url)
{

  switch(req_type) {
  case BYTES_RANDOM_NUMBER:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
             api_types[req_type].domain_address,
             api_types[req_type].samples);
    break;
  case INT16_RANDOM_NUMBER:
  case INT32_RANDOM_NUMBER:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
             api_types[req_type].domain_address,
             api_types[req_type].min_range_i,
             api_types[req_type].max_range_i,
             api_types[req_type].samples);
    break;
  case DOUBLE_RANDOM_NUMBER:
  case FLOAT_RANDOM_NUMBER:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
             api_types[req_type].domain_address,
             api_types[req_type].min_range_f,
             api_types[req_type].max_range_f,
             api_types[req_type].samples);
    break;
  case STREAM_BINARY:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
             api_types[req_type].domain_address,
             api_types[req_type].samples);
    break;
  case PERFORMANCE_REQUEST:
    break;
  case FIRMWARE_INFO_REQUEST:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
	    api_types[req_type].domain_address);
    break;
  case SYSTEM_INFO_REQUEST:
    snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
	    api_types[req_type].domain_address);
    break;
  default:
    break;
  }

}


size_t curl_write_cbk(void *content, size_t size, size_t nmemb, void *userp)
{
#ifdef NO_DYNAMIC_MEMORY_ALLOCATION
  size_t real_size = size *nmemb;
  memory_t *mem = (memory_t *)userp;
  size_t available_space = TOTAL_MAX_SIZE_RESPONSE - (mem->size + real_size + 1);
  if (available_space > (real_size + 1)) {
    memcpy(&(mem->memory[mem->size]), content, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;
    return real_size;
  }
  fprintf(stderr, "Static buffer full!\n");
  return 0;
#else
    size_t real_size = size * nmemb;
    memory_t *mem = (memory_t *)userp;
    char *ptr = realloc(mem->memory, mem->size + real_size + 1);
    if (!ptr) {
	fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
	return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), content, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;
    return real_size;
#endif
}


void parse_response_string(char *random_values_string, void *buffer, size_t samples, e_req_type_t request_type)
{
    /* Skip first character because it's [ */
    random_values_string ++;
    /* Skip last character because is ] */
    random_values_string[strlen(random_values_string)-1]=0;
    char *token = strtok(random_values_string,",");
    size_t i = 0;

    switch(request_type) {
        case BYTES_RANDOM_NUMBER:  
            for (i = 0; i < samples && token != NULL; i++) {
                /* Remove quotes */
                token++;
                token[strlen(token) - 1] = '\0';
                uint8_t value = (uint8_t)strtol(token, NULL, 16);
                ((uint8_t *)buffer)[i] = value;
                token = strtok(NULL, ",");
            }
            break;
        case INT16_RANDOM_NUMBER:
            for (i = 0; i < samples; i++) {
                ((int16_t *)buffer)[i] = (int16_t)strtol(token, NULL, 10);
                token = strtok(NULL, ",");
            }
            break;
        case INT32_RANDOM_NUMBER:
            for (i = 0; i < samples; i++) {
                ((int32_t *)buffer)[i] = strtol(token, NULL, 10);
                token = strtok(NULL, ",");
            }
            break;
        case DOUBLE_RANDOM_NUMBER:
            for (i = 0; i < samples; i++) {
                ((double *)buffer)[i] = strtod(token, NULL);
                token = strtok(NULL, ",");
            }
            break;
        case FLOAT_RANDOM_NUMBER:
            for (i = 0; i < samples; i++) {
                ((float *)buffer)[i] = strtof(token, NULL);
                token = strtok(NULL, ",");
            }
            break;
        default:
            break;
    }
}
