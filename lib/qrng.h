#ifndef QRNG_H
#define QRNG_H




int qrng_open(const char *device_domain_address); 
int qrng_random_stream(FILE *stream, size_t size);
int qrng_random_double(double min, double max, size_t samples, double *buffer);
int qrng_random_float(float min, float max, size_t samples, float *buffer);

int qrng_random_int64(int64_t min, int64_t max, size_t samples, int64_t *buffer);
int qrng_random_int32(int32_t min, int32_t max, size_t samples, int32_t *buffer);

void qrng_close();

#endif
