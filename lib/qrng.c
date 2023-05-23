#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "qrng.h"


#define IPV4_IP_LENGTH 16u
#define URL_MAX_LENGTH 256u
#define DEFAULT_NUMBER_OF_SAMPLES 1u
#define MIN_VALUE_INT 0u
#define MAX_VALUE_INT 1u

#define MIN_VALUE_FLOAT 0.0f
#define MAX_VALUE_FLOAT 1.0f


typedef enum {
  SHORT_RANDOM_NUMBER = 0,
  INT_RANDOM_NUMBER,
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
  size_t samples;
  int64_t min_range_i;
  int64_t max_range_i;
  double min_range_f;
  double max_range_f;
}s_api_t;

typedef struct {
    char *memory;
    size_t size;
}memory_t;


static const s_api_t api_types[]={
  {SHORT_RANDOM_NUMBER,  ""                                                      , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {INT_RANDOM_NUMBER,    "https://%s/api/2.0/int?min=%lu&max=%lu&quantity=%lu"   , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {DOUBLE_RANDOM_NUMBER, "https://%s/api/2.0/double?min=%lf&max=%lf&quantity=%lu", DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {FLOAT_RANDOM_NUMBER,  ""                                                      , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {STREAM_BINARY,        "https://%s/api/2.0/streambytes?size=%lu"               , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT}, 
  {PERFORMANCE_REQUEST,  ""                                                      , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {FIRMWARE_INFO_REQUEST,""                                                      , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT},
  {SYSTEM_INFO_REQUEST,  ""                                                      , DEFAULT_NUMBER_OF_SAMPLES, MIN_VALUE_INT, MAX_VALUE_INT, MIN_VALUE_FLOAT, MAX_VALUE_FLOAT}
};


static char ip_address[IPV4_IP_LENGTH] = {0};

static CURL *p_curl_handle = NULL;

static size_t curl_write_cbk(void *content, 
		      size_t size, 
		      size_t nmemb, 
		      void *userp);
static int create_req_url(e_req_type_t req_type, char *api_url);
static int execute_request();


int qrng_open(const char *device_ip_address){

    int retval = 0;

    strncpy(ip_address, device_ip_address, IPV4_IP_LENGTH);
     

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
	} else {

#ifdef DEBUG
	    (void)curl_easy_setopt(p_curl_handle, CURLOPT_VERBOSE, 1L);
#endif
	    (void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	    (void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

	    	    
	    (void)curl_easy_setopt(p_curl_handle, CURLOPT_FAILONERROR, 1L);
	}
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
  return 0;
}


int qrng_random_double(double min, double max, size_t samples, double *buffer)
{
  return 0;
}

int qrng_random_float(float min, float max, size_t samples, float *buffer)
{
  return 0;
}

int qrng_random_int64(int64_t min, int64_t max, size_t samples, int64_t *buffer)
{

}

int qrng_random_int32(int32_t min, int32_t max, size_t samples, int32_t *buffer)
{
    int retval = 0;

    CURLcode error = CURLE_OK;    

    char final_url[256]={0};

    //allocate only 1 char, the callback will allocate more memory
    memory_t mem_buffer;
    mem_buffer.memory = malloc(sizeof(char));

    if (mem_buffer.memory) {
      memset(&mem_buffer, 0, sizeof(mem_buffer));
        
      if(!(retval=create_req_url(SHORT_RANDOM_NUMBER, final_url))) {
      
        retval = execute_request(final_url, (void *)(&mem_buffer));
      
        if (!retval) {
          //parse values array
          char *random_values_string = mem_buffer.memory;
          /* Skip first character because it's [ */
          random_values_string ++;
          /* Skip last character because is ] */
          random_values_string[strlen(random_values_string)-1]=0;
          char *token = strtok(random_values_string,",");
          size_t i = 0;
          
          for (i = 0; i < samples; i++) {
            buffer[i] = atol(token);
            token = strtok(NULL, ",");
          }   
        }
        else {
          fprintf(stderr, "could not execute curl request");
        }
      }
      else{
        fprintf(stderr, "reqeuest url could not be created");
      }
    }
    else {
      retval = -1;
    }
    if (mem_buffer.memory) {
      free(mem_buffer.memory);
    }
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




int create_req_url(e_req_type_t req_type, char *api_url)
{
  int retval = 0;

  if (ip_address[0] != '\0') {
    retval = -1;
  }
  else {
    switch(req_type) {
    case SHORT_RANDOM_NUMBER:
    case INT_RANDOM_NUMBER:
      snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
               ip_address,
               api_types[req_type].min_range_i,
               api_types[req_type].max_range_i,
               api_types[req_type].samples);
      break;
    case DOUBLE_RANDOM_NUMBER:
    case FLOAT_RANDOM_NUMBER:
      snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
               ip_address,
               api_types[req_type].min_range_f,
               api_types[req_type].max_range_f,
               api_types[req_type].samples);
      break;
    case STREAM_BINARY:
      snprintf(api_url, URL_MAX_LENGTH, api_types[req_type].api_url,
               ip_address,
               api_types[req_type].samples);
      break;
    case PERFORMANCE_REQUEST:
      break;
    case FIRMWARE_INFO_REQUEST:
      break;
    case SYSTEM_INFO_REQUEST:
      break;
    default:
      break;
    }
  }
  return retval;
}


size_t curl_write_cbk(void *content, size_t size, size_t nmemb, void *userp)
{
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
