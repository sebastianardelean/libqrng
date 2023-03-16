#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/random.h>



struct memory_t {
    char *memory;
    size_t size;
};

struct pool_t {
    size_t entropy_count;
    size_t buf_size;
    uint32_t *buf;
};

static size_t 
write_memory_callback(void *content, size_t size, size_t nmemb, void *userp)
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

int main(int argc, char **argv)
{
    CURL *p_curl_handle;
    CURLcode error;
    int fd = 0;
    int retval = 0;
    struct memory_t mem_buffer;
    struct pool_t entropy_pool;

    memset(&mem_buffer, 0, sizeof(struct memory_t));
    memset(&entropy_pool, 0, sizeof(struct pool_t));

    mem_buffer.memory = malloc(1);
    if (!mem_buffer.memory) {
	fprintf(stderr, "Error allocating memory");
	retval = 1;
	goto exit_prog;
    }


    if(curl_global_init(CURL_GLOBAL_ALL)!=0) {
	fprintf(stderr, "Error in curl_global_init");
	retval = 2;
	goto exit_curl;
    }
  
   
    p_curl_handle = curl_easy_init();
    if(p_curl_handle) {
	(void)curl_easy_setopt(p_curl_handle, CURLOPT_URL, argv[1]);
	/*Setup verbose*/
#ifdef DEBUG
	(void)curl_easy_setopt(p_curl_handle, CURLOPT_VERBOSE, 1L);
#endif
	(void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	(void)curl_easy_setopt(p_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

	(void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);

	(void)curl_easy_setopt(p_curl_handle, CURLOPT_WRITEDATA, (void *)&mem_buffer);
    
	/* Perform the request, res will get the return code */
	error = curl_easy_perform(p_curl_handle);
	
	/* Check for errors */
	if(error != CURLE_OK) {
	    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(error));
	    retval = 3;
	    goto exit_curl_perform;
	}
	else {
	    /* Do nothing */
	    fd = open("/dev/random", O_WRONLY);
	    if (fd == -1) {
		fprintf(stderr, "FD open: %s", strerror(errno));
		retval = 4;
		goto exit_fd_error;
	    }

    
	    printf("%lu bytes retrieved\n", (unsigned long)mem_buffer.size);
	    
	    char *random_values_array = mem_buffer.memory;
	    /*Skip first character because is [*/
	    random_values_array++;
	    /*Skip last character because is ]*/
	    random_values_array[strlen(random_values_array)-1] = 0;

	    char *delimiter = ",";
	    char *token = strtok(random_values_array, delimiter);
	    size_t i = 0;
	    entropy_pool.buf_size++;
	    
	    entropy_pool.buf = malloc(entropy_pool.buf_size * sizeof(uint32_t));
	    if (!entropy_pool.buf) {
		fprintf(stderr, "Could not allocate memory for entropy pool");
		retval = 5;
		goto exit_entropy_pool;
	    }
	    
	    do {
		
		uint32_t *ptr = realloc(entropy_pool.buf, entropy_pool.buf_size*sizeof(uint32_t));
		if (!ptr) {
		    fprintf(stderr, "Could not reallocate memory for entropy pool");
		    retval = 7;
		    goto exit_full;
		}
		
		entropy_pool.buf = ptr;
		entropy_pool.buf[i] = atol(token);
		printf("%u ",entropy_pool.buf[i]);
		
		entropy_pool.buf_size++;
		i++;
		
		
		token = strtok(NULL, delimiter);
	    } while (token != NULL); 

	    entropy_pool.buf_size--;
	    printf("\nRetrieved %lu values\n", entropy_pool.buf_size); 
	    entropy_pool.entropy_count = entropy_pool.buf_size * sizeof(uint32_t)*8;//number of bits
	

	    if(ioctl(fd, RNDADDENTROPY, &entropy_pool)<0) {
		fprintf(stderr, "%s", strerror(errno));
		retval = 6;
		goto exit_full;
	    }
	    
	}
    }

exit_full:
    free(entropy_pool.buf);
exit_entropy_pool:
    close(fd);
exit_fd_error:
exit_curl_perform:
    curl_easy_cleanup(p_curl_handle);
exit_curl:
    curl_global_cleanup();
    free(mem_buffer.memory); 
exit_prog:
    return retval;
}
