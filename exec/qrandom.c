#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "qrng.h"


/* The official name of this program (e.g., no `g' prefix).  */
#define PROGRAM_NAME "qrand"

#define VERSION "1.0.0"

#define AUTHORS "Sebastian M. Ardelean"


#define DEFAULT_NUMBER_OF_SAMPLES 10u
#define DEFAULT_MIN_VALUE_F 0.0f
#define DEFAULT_MAX_VALUE_F 1.0f
#define DEFAULT_MIN_VALUE_I 0
#define DEFAULT_MAX_VALUE_I 1
#define DEFAULT_RANDOM_VALUE_TYPE 1

#define IPV4_IP_LENGTH 16u



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


static const char default_ip_address[]="10.17.2.72";

static void print_help();

int main(int argc, char **argv)
{

  int opt = -1;
  char ip_address[IPV4_IP_LENGTH] = "\0";
  int retval = 0;
  int32_t *data32 = NULL;
  int64_t *data64 = NULL;
  float *dataf = NULL;
  double *datad = NULL;
  size_t i = 0;
   
  
  strncpy(ip_address, default_ip_address, IPV4_IP_LENGTH);
  
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
  while ((opt = getopt(argc, argv, "ha:s:m:M:i:I:")) != -1) {
    switch (opt) {
    case 'h':
      print_help();
      break;
    case 'a':
      strncpy(ip_address, optarg, strlen(optarg));
      break;
    case 't':
      random_number_type = atoi(optarg);
      break;
    case 's':
      number_of_samples = atol(optarg);
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
    default:
      print_help();
      exit(EXIT_FAILURE);
    }
  }

  
  printf("IP Address: %s\n", ip_address);
  printf("Number of samples: %u\n", number_of_samples);
  printf("Interval [%lf %lf)\n", min_value_f, max_value_f);


  /*Initialize qrng library*/
  retval = qrng_open(ip_address);
  if (retval) {
    exit(EXIT_FAILURE);
  }
  
  switch (random_number_type) {
  case SHORT_RANDOM_NUMBER:
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
  exit(EXIT_SUCCESS);
}


void print_help(void)
{
  fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
  fprintf(stderr, "\n\tAuthors:%s\n\n", AUTHORS);
  fprintf(stderr, "%s [-h] [-a ip] [-s] [-m] [-M] [-i] [-I]\n", PROGRAM_NAME);
  fprintf(stderr, "-h \t help\n");
  fprintf(stderr, "-a \t ip address\n");
  fprintf(stderr, "-s \t number of samples\n");
  fprintf(stderr, "-m \t min value double\n");
  fprintf(stderr, "-M \t max value double\n");
  fprintf(stderr, "-i \t min value int64\n");
  fprintf(stderr, "-I \t max value int64\n");
  fprintf(stderr, "-t \t type\n");
  

  
}


 
