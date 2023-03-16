#ifndef QRNG_H
#define QRNG_H

typedef struct {
    char *memory;
    size_t size;
}memory_t;

int qrng_open(const char *device_ip_address); 
int qrng_random_stream(FILE *stream, void (*progress_cbk)(size_t now, size_t total));


int qrng_random_u32(uint32_t min, uint32_t max, size_t samples, memory_t *buffer);

void qrng_close();

#endif
