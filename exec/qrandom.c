#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "qrng.h"


/*TODO:
 * 1. Default type to 7 -> to be implemented. Do not exit failure
 * 2. Check ranges for min/max values
 * 3. Check for samples to not be 0*/

#define PROGRAM_NAME "qrand"

#define VERSION "1.0.0"

#define AUTHORS "Sebastian M. Ardelean"


#ifdef DEBUG
#define debug_print(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
	__LINE__, __func__, __VA_ARGS__); } while (0)
#else
#define debug_print
#endif

#define DEFAULT_NUMBER_OF_SAMPLES 1u
#define DEFAULT_MIN_VALUE_F 0.0f
#define DEFAULT_MAX_VALUE_F 1.0f
#define DEFAULT_MIN_VALUE_I 0
#define DEFAULT_MAX_VALUE_I 1
#define DEFAULT_RANDOM_VALUE_TYPE 7

#define DOMAIN_ADDR_LENGTH 256u

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
}e_rand_number_t;


static const char default_domain_addr[]="random.cs.upt.ro";

static void print_help();

int main(int argc, char **argv)
{

    int opt = -1;
    char domain_addr[DOMAIN_ADDR_LENGTH] = "\0";
    int retval = 0;
    int32_t *data32 = NULL;
    int64_t *data64 = NULL;
    float *dataf = NULL;
    double *datad = NULL;
    size_t i = 0;
    FILE *stream = NULL;

    strncpy(domain_addr, default_domain_addr, DOMAIN_ADDR_LENGTH);

    uint32_t number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
    double min_value_f = DEFAULT_MIN_VALUE_F;
    double max_value_f = DEFAULT_MAX_VALUE_F;
    int64_t min_value_i = DEFAULT_MIN_VALUE_I;
    int64_t max_value_i = DEFAULT_MAX_VALUE_I;
    e_rand_number_t random_number_type = DEFAULT_RANDOM_VALUE_TYPE;



    if (argc == 1) {
        print_help();
        exit(EXIT_FAILURE);

    }
    while ((opt = getopt(argc, argv, "ha:s:m:M:i:I:t:f:")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                strncpy(domain_addr, optarg, strlen(optarg));
                break;
            case 't':
                random_number_type = atoi(optarg);
                break;
            case 's':
                number_of_samples = atol(optarg);
		if (number_of_samples < 1) {
		    number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
		}
                break;
            case 'm':
                min_value_f = atof(optarg);
                break;
            case 'M':
                max_value_f = atof(optarg);
                break;
            case 'i':
                min_value_i = atoll(optarg);
                break;
            case 'I':
                max_value_i = atoll(optarg);
                break;
	    case 'f':
		stream = fopen(optarg, "wb");
		break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }
    if (min_value_f > max_value_f) {
	print_help();
	exit(EXIT_FAILURE);
    }
    if (min_value_i > max_value_i) {
	print_help();
	exit(EXIT_FAILURE);
    }

    debug_print("Domain address: %s\n", domain_addr);
    /*Initialize qrng library*/
    retval = qrng_open(domain_addr);
    if (retval) {
        exit(EXIT_FAILURE);
    }

    switch (random_number_type) {
        case SHORT_RANDOM_NUMBER:
            debug_print("Short random number, %d samples %d %d\n",number_of_samples,min_value_i, max_value_i);
            data32 = malloc(number_of_samples * sizeof(int32_t));
            if (data32) {
                if (qrng_random_int32(min_value_i, max_value_i, number_of_samples, data32) == 0) {
                    //print the value to stdout
                    for (i = 0; i < number_of_samples; i++) {
                        printf("%d ", data32[i]);
                    }
                }

            }
            break;
        case INT_RANDOM_NUMBER:
            data64 = malloc(number_of_samples * sizeof(int64_t));
            if (data64) {
                if (qrng_random_int64(min_value_i, max_value_i, number_of_samples, data64) == 0) {
                    //print the value to stdout
                    for (i = 0; i < number_of_samples; i++) {
                        printf("%ld ", data64[i]);
                    }
                }
            }
            break;
        case DOUBLE_RANDOM_NUMBER:
            datad = malloc(number_of_samples * sizeof(double));
            if (datad) {
                if (qrng_random_double(min_value_f, max_value_f, number_of_samples, datad) == 0) {
                    //print the value to stdout
                    for (i = 0; i < number_of_samples; i++) {
                        printf("%lf ", datad[i]);
                    }
                }
            }
            break;
        case FLOAT_RANDOM_NUMBER:
            dataf = malloc(number_of_samples * sizeof(float));
            if (dataf) {
                if (qrng_random_float(min_value_f, max_value_f, number_of_samples, dataf) == 0) {
                    //print the value to stdout
                    for (i = 0; i < number_of_samples; i++) {
                        printf("%f ", dataf[i]);
                    }
                }
            }
            break;
        case STREAM_BINARY:
	    if (stream == NULL) {
		stream = stdout;
	    }
	    (void)qrng_random_stream(stream, number_of_samples);
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

    qrng_close();
    if (data32 != NULL) {
        free(data32);
    }

    if (data64 != NULL) {
        free(data64);
    }

    if (dataf != NULL) {
        free(dataf);
    }

    if (datad != NULL) {
        free(datad);
    }

    if (stream != NULL) {
	fclose(stream);
    }
    exit(EXIT_SUCCESS);
}


void print_help(void)
{
    fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
    fprintf(stderr, "\n\tAuthors:%s\n\n", AUTHORS);
    fprintf(stderr, "%s [-h] [-a domain] [-s no of samples] [-m min double value] [-M max double value] [-i min int value] [-I max int value] [-t type] [-f stream]\n", PROGRAM_NAME);
    fprintf(stderr, "-h \t help\n");
    fprintf(stderr, "-a \t domain address. [Default: random.cs.upt.ro]\n");
    fprintf(stderr, "-s \t number of samples. [Default 1]\n");
    fprintf(stderr, "-m \t min value double. [Default 0.0]\n");
    fprintf(stderr, "-M \t max value double. [Default 1.0]\n");
    fprintf(stderr, "-i \t min value int64. [Default 0]\n");
    fprintf(stderr, "-I \t max value int64. [Default 1]\n");
    fprintf(stderr, "-t \t type. [Default 7]\n");
    fprintf(stderr, "-f \t stream.\n");
    fprintf(stderr, "\n================================\n");
    fprintf(stderr, "Possible values for t:\n");
    fprintf(stderr, "\t0 -> 32-bit integer\n");
    fprintf(stderr, "\t1 -> 64-bit integer\n");
    fprintf(stderr, "\t2 -> double value\n");
    fprintf(stderr, "\t3 -> float value\n");
    fprintf(stderr, "\t4 -> stream of bytes\n");
    fprintf(stderr, "\t5 -> device performance\n");
    fprintf(stderr, "\t6 -> firmware info\n");
    fprintf(stderr, "\t7 -> system info\n");

}

