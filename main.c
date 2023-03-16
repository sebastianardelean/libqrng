#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/random.h>

#include "qrng.h"

struct pool_t {
    size_t entropy_count;
    size_t buf_size;
    uint32_t *buf;
};


int main(int argc, char **argv)
{
    int retval = 0;

    int fd;

    memory_t buffer;
    struct pool_t entropy_pool;

    memset(&entropy_pool, 0, sizeof(struct pool_t));

    memset(&buffer, 0, sizeof(memory_t));

    buffer.memory = malloc(sizeof(char));

    char ip_address[]="10.17.2.72";

    retval = qrng_open(ip_address);

    if (retval) {
	free(buffer.memory);
	qrng_close();
	return retval;
    }

    retval = qrng_random_u32(1, 120, 2, &buffer);

    if (retval) {
	free(buffer.memory);
	qrng_close();
	return retval;
    }

    fd = open("/dev/random", O_WRONLY);
    if (fd == -1) {
	fprintf(stderr, "FD open: %s", strerror(errno));
	free(buffer.memory);
	qrng_close();
	return -1;
    }

    char *random_values_array = buffer.memory;
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
	free(buffer.memory);
	close(fd);
	qrng_close();
	return -1;
    }
	    
    do {
		
	uint32_t *ptr = realloc(entropy_pool.buf, entropy_pool.buf_size*sizeof(uint32_t));
	if (!ptr) {
	    fprintf(stderr, "Could not reallocate memory for entropy pool");
	    free(buffer.memory);
	    close(fd);
	    free(entropy_pool.buf);
	    return -1;
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
	free(buffer.memory);
	close(fd);
	free(entropy_pool.buf);
	qrng_close();
	return -1;
    }

	

    free(buffer.memory);
    free(entropy_pool.buf);
    close(fd);
    qrng_close();
    return 0;
}
