#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "qrng.h"

//https://10.17.2.72/api/2.0/int?min=1&max=100&quantity=10"



static const char *API_INTEGERS_IN_RANGE="https://%s/api/2.0/int?min=%lu&max=%lu&quantity=%lu";

static char ip_address[16] = {0};

static CURL *p_curl_handle = NULL;


static int curl_progress_cbk(void *clientp,
                      curl_off_t dltotal,
                      curl_off_t dlnow,
                      curl_off_t ultotal,
                      curl_off_t ulnow);

static size_t curl_write_cbk(void *content, 
		      size_t size, 
		      size_t nmemb, 
		      void *userp);


static void (*curl_progress_callback)(size_t now, size_t total) = NULL;

int qrng_open(const char *device_ip_address){
    
    int retval = 0;

    strncpy(ip_address, device_ip_address, 16);
     

    if(curl_global_init(CURL_GLOBAL_ALL)!=0) {
	fprintf(stderr, "Error in curl_global_init");
	retval = -1;
	
    }
    else {
	p_curl_handle = curl_easy_init();
	if (!p_curl_handle) {
	    fprintf(stderr, "Error in curl_easy_init");
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
    }
    curl_global_cleanup();
}


int qrng_random_stream(FILE *stream, void (*progress_cbk)(size_t now, size_t total))

{
    FILE *f;
    (void)curl_easy_setopt(p_curl_handle, CURLOPT_XFERINFODATA, f);

    if (progress_cbk) {
	(void)curl_easy_setopt(p_curl_handle, CURLOPT_XFERINFOFUNCTION, &curl_progress_cbk);
    }
    
    curl_progress_callback = progress_cbk;
}

int qrng_random_u32(uint32_t min, uint32_t max, size_t samples, memory_t *buffer)
{
    int retval = 0;

    CURLcode error = CURLE_OK;    

    char final_url[256]={0};

    
    

    if (ip_address[0] != '\0') {
	snprintf(final_url, 256, API_INTEGERS_IN_RANGE, ip_address, min, max, samples);

        (void)curl_easy_setopt(p_curl_handle, CURLOPT_URL, final_url);
	(void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEFUNCTION, &curl_write_cbk);
        (void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEDATA, (void *)buffer);

	error = curl_easy_perform(p_curl_handle);


	if(error != CURLE_OK) {
	    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(error));
	    retval = -1;
	    goto exit_curl_perform;
	}

    } else {
	retval = -2;
    }
    return retval;
}



int curl_progress_cbk(void *clientp,
                      curl_off_t dltotal,
                      curl_off_t dlnow,
                      curl_off_t ultotal,
                      curl_off_t ulnow)
{
    if (curl_progress_callback) {
	curl_progress_callback(dlnow, dltotal);
    }
}

size_t curl_write_cbk(void *content, size_t size, size_t nmemb, void *userp)
{
    size_t real_size = size * nmemb;
    struct memory_t *mem = (struct memory_t *)userp;
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
}

