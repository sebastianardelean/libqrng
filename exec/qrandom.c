#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <qrng.h>


/* The official name of this program (e.g., no `g' prefix).  */
#define PROGRAM_NAME "qrand"

#define VERSION "1.0.0"

#define AUTHORS "Sebastian M. Ardelean"


#define DEFAULT_NUMBER_OF_SAMPLES 10u
#define DEFAULT_MIN_VALUE 0.0f
#define DEFAULT_MAX_VALUE 1.0f
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

  strncpy(ip_address, default_ip_address, IPV4_IP_LENGTH);
  
  uint32_t number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
  double min_value = DEFAULT_MIN_VALUE;
  double max_value = DEFAULT_MAX_VALUE;
  e_rand_number_t random_number_type = DEFAULT_RANDOM_VALUE_TYPE; 

  
  
  if (argc == 1) {
    print_help();
    exit(EXIT_FAILURE);
    
  }
  while ((opt = getopt(argc, argv, "hi:s:m:M:t:")) != -1) {
    switch (opt) {
    case 'h':
      print_help();
      break;
    case 'i':
      strncpy(ip_address, optarg, strlen(optarg));
      break;
    case 't':
      random_number_type = atoi(optarg);
      break;
    case 's':
      number_of_samples = atol(optarg);
      break;
    case 'm':
      min_value = atof(optarg);
      break;
    case 'M':
      max_value = atof(optarg);
      break;
    default:
      print_help();
      exit(EXIT_FAILURE);
    }
  }


  printf("IP Address: %s\n", ip_address);
  printf("Number of samples: %u\n", number_of_samples);
  printf("Interval [%lf %lf)\n", min_value, max_value);


  /*Initialize qrng library*/
  retval = qrng_open(ip_address);
  if (retval) {
    exit(EXIT_FAILURE);
  }
  
  switch (random_number_type) {
  case SHORT_RANDOM_NUMBER:
    break;
  case INT_RANDOM_NUMBER:
    break;
  case DOUBLE_RANDOM_NUMBER:
    break;
  case FLOAT_RANDOM_NUMBER:
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
    exit(EXIT_FAILURE);
  }

  qrng_close();
  exit(EXIT_SUCCESS);
}


void print_help(void)
{
  fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
  fprintf(stderr, "\n\tAuthors:%s\n\n", AUTHORS);
  fprintf(stderr, "%s [-h] [-i ip] [-u] [-t] [-s] [-m] [-M]\n", PROGRAM_NAME);
  fprintf(stderr, "-h \t help\n");
  fprintf(stderr, "-i \t ip address\n");
  fprintf(stderr, "-s \t number of samples\n");
  fprintf(stderr, "-m \t min value\n");
  fprintf(stderr, "-M \t max value\n");
  fprintf(stderr, "-t \t type\n");
  

  
}


 
