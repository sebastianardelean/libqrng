/****************************************************************************
 * randbytes - Quantum Random Number Generator using IDQ's Quantis Appliance  *
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
 * @file randbytes.c
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
#define PROGRAM_NAME "randbytes"

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
 * @def DEFAULT_MIN_VALUE_I
 * @brief A macro for defining the default minimum integer number.
 *
 */
#define DEFAULT_MIN_VALUE_I 0

/**
 * @def DEFAULT_MAX_VALUE_I
 * @brief A macro for defining the default maximum integer number.
 *
 */
#define DEFAULT_MAX_VALUE_I 100

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
    uint8_t *data8 = NULL;

    size_t i = 0;




    uint32_t number_of_samples = DEFAULT_NUMBER_OF_SAMPLES;


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
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }





    /*Initialize qrng library*/
    retval = qrng_open(domain_addr);
    if (retval) {
        exit(EXIT_FAILURE);
    }


    data8 = malloc(number_of_samples * sizeof(uint8_t));
    if (data8) {
      if (qrng_random_bytes(number_of_samples, data8) == 0) {
        //print the value to stdout
        for (i = 0; i < number_of_samples; i++) {
          printf("%x ", data8[i]);
        }
      }
      
    }


    qrng_close();
    if (data8 != NULL) {
        free(data8);
    }

    exit(EXIT_SUCCESS);
}


void print_help(void)
{
    fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
    fprintf(stderr, "%s [-h] [-a domain] [-s no of samples] [-i min int value] [-I max int value]\n", PROGRAM_NAME);
    fprintf(stderr, "-h \t help\n");
    fprintf(stderr, "-a \t domain address.\n");
    fprintf(stderr, "-s \t number of samples. [Default 1]\n");
}

