/****************************************************************************
 * randint64 - Quantum Random Number Generator using IDQ's Quantis Appliance  *
 *                                                                          *
 * Copyright (C) 2023  Sebastian Mihai Ardelean                             *
 *                                                                          *
 * This program is free software: you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation, either version 3 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * This program is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 ****************************************************************************/

/**
 * @file randint64.c
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 24 May 2023
 * @brief API for interacting with IDQ's Quantis Appliance
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <qrng.h>





#define PROGRAM_NAME "randint64"

#define VERSION "1.0.0"

#define AUTHORS "Sebastian M. Ardelean"




#define DEFAULT_NUMBER_OF_SAMPLES 1u

#define DEFAULT_MIN_VALUE_I 0
#define DEFAULT_MAX_VALUE_I 100

#define DOMAIN_ADDR_LENGTH 256u


static void print_help();

int main(int argc, char **argv)
{

    int opt = -1;
    char domain_addr[DOMAIN_ADDR_LENGTH] = "\0";
    int retval = 0;
    int64_t *data64 = NULL;
    size_t i = 0;




    uint32_t number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
    int64_t min_value_i = DEFAULT_MIN_VALUE_I;
    int64_t max_value_i = DEFAULT_MAX_VALUE_I;




    if (argc == 1) {
        print_help();
        exit(EXIT_FAILURE);

    }
    while ((opt = getopt(argc, argv, "ha:s:i:I:")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                strncpy(domain_addr, optarg, strlen(optarg));
                break;
            case 's':
                number_of_samples = atol(optarg);
		if (number_of_samples < 1) {
		    number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
		}
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


    if (min_value_i > max_value_i) {
	print_help();
	exit(EXIT_FAILURE);
    }


    /*Initialize qrng library*/
    retval = qrng_open(domain_addr);
    if (retval) {
        exit(EXIT_FAILURE);
    }


    data64 = malloc(number_of_samples * sizeof(int64_t));
    if (data64) {
      if (qrng_random_int64(min_value_i, max_value_i, number_of_samples, data64) == 0) {
        //print the value to stdout
        for (i = 0; i < number_of_samples; i++) {
          printf("%ld ", data64[i]);
        }
      }
    }
    

    qrng_close();
    

    if (data64 != NULL) {
        free(data64);
    }

    
    exit(EXIT_SUCCESS);
}


void print_help(void)
{
    fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
    fprintf(stderr, "%s [-h] [-a domain] [-s no of samples] [-i min int value] [-I max int value]\n", PROGRAM_NAME);
    fprintf(stderr, "-h \t help\n");
    fprintf(stderr, "-a \t domain address. [Default: random.cs.upt.ro]\n");
    fprintf(stderr, "-s \t number of samples. [Default 1]\n");
    fprintf(stderr, "-i \t min value int64. [Default 0]\n");
    fprintf(stderr, "-I \t max value int64. [Default 100]\n");

}

