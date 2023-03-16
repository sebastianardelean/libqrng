#ifndef QRNG_H
#define QRNG_H



typedef struct {
    char *memory;
    size_t size;
}memory_t;

int qrng_open(const char *device_ip_address); 
int qrng_random_stream(FILE *stream, size_t size, int (*progress_cbk)(void *clientp, int64_t dltotal, int64_t dlnow, int64_t ultotal, int64_t ulnow));
int qrng_random_double(uint32_t min, uint32_t max, size_t samples, memory_t *buffer);
int qrng_random_u32(uint32_t min, uint32_t max, size_t samples, memory_t *buffer);

void qrng_close();

#endif
