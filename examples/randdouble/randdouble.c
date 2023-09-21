/****************************************************************************
 * randdouble - Quantum Random Number Generator using IDQ's Quantis Appliance  *
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
 * @file randdouble.c
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




/**
 * @def PROGRAM_NAME
 * @brief A macro for the program name.
 *
 */
#define PROGRAM_NAME "randdouble"

/**
 * @def VERSION
 * @brief A macro for the program version.
 *
 */
#define VERSION "1.0.0"

/**
 * @def AUTHORS
 * @brief A macro for the author.
 *
 */
#define AUTHORS "Sebastian M. Ardelean"

/**
 * @def DEFAULT_NUMBER_OF_SAMPLES
 * @brief A macro for defining the default number of samples to request.
 *
 */
#define DEFAULT_NUMBER_OF_SAMPLES 1u

/**
 * @def DEFAULT_MIN_VALUE_F
 * @brief A macro for defining the default minimum float number.
 *
 */
#define DEFAULT_MIN_VALUE_F 0.0f

/**
 * @def DEFAULT_MAX_VALUE_F
 * @brief A macro for defining the default maximum float number.
 *
 */
#define DEFAULT_MAX_VALUE_F 1.0f

/**
 * @def DOMAIN_ADDR_LENGTH
 * @brief A macro for defining the IDQ's Quantis Appliance domain name address.
 *
 */
#define DOMAIN_ADDR_LENGTH 256u

/**
 * @brief Print the help (command line options) for this program.
 *
 */
static void print_help(void);

int main(int argc, char **argv)
{

    int opt = -1;
    char domain_addr[DOMAIN_ADDR_LENGTH] = "\0";
    int retval = 0;

    double *datad = NULL;
    size_t i = 0;




    uint32_t number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;
    double min_value_f = DEFAULT_MIN_VALUE_F;
    double max_value_f = DEFAULT_MAX_VALUE_F;




    if (argc == 1) {
        print_help();
        exit(EXIT_FAILURE);

    }
    while ((opt = getopt(argc, argv, "ha:s:m:M:")) != -1) {
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
            case 'm':
                min_value_f = atof(optarg);
                break;
            case 'M':
                max_value_f = atof(optarg);
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

    /*Initialize qrng library*/
    retval = qrng_open(domain_addr);
    if (retval) {
        exit(EXIT_FAILURE);
    }

    datad = malloc(number_of_samples * sizeof(double));
    if (datad) {
      if (qrng_random_double(min_value_f, max_value_f, number_of_samples, datad) == 0) {
        //print the value to stdout
        for (i = 0; i < number_of_samples; i++) {
          printf("%lf ", datad[i]);
        }
      }
    }



    qrng_close();
    

    if (datad != NULL) {
        free(datad);
    }

    
    exit(EXIT_SUCCESS);
}


void print_help(void)
{
    fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
    fprintf(stderr, "%s [-h] [-a domain] [-s no of samples] [-m min double value] [-M max double value]\n", PROGRAM_NAME);
    fprintf(stderr, "-a \t domain address.\n");
    fprintf(stderr, "-s \t number of samples. [Default 1]\n");
    fprintf(stderr, "-m \t min value double. [Default 0.0]\n");
    fprintf(stderr, "-M \t max value double. [Default 1.0]\n");

}

