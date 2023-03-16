#ifndef QRNG_H
#define QRNG_H

int qrng_open(const char *url); 
int qrng_random_stream(FILE *stream, void (*progress_cbk)(size_t now, size_t total));


int qrng_random_u32(int min, int max, size_t samples);

void qrng_close();

#endif
