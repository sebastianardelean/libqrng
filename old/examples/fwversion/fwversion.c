/****************************************************************************
 * fwversion -
 *           Quantum Random Number Generator using IDQ's Quantis Appliance  *
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
 * @file fwversion.c
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 31 May 2023
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
#define PROGRAM_NAME "fwversion"

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


    if (argc == 1) {
        print_help();
        exit(EXIT_FAILURE);

    }
    /*Parse the command line arguments*/
    while ((opt = getopt(argc, argv, "ha:")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                strncpy(domain_addr, optarg, strlen(optarg));
                break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    /* Initialize the qrng library */
    retval = qrng_open(domain_addr);
    if (retval) {
        exit(EXIT_FAILURE);
    }

    /* Request firmware info and redirect the stream to stdout */
    (void)qrng_firmware_info(stdout);

    printf("\n");

    qrng_close();
    
    exit(EXIT_SUCCESS);
}


void print_help(void)
{
    fprintf(stderr, "\n\n\t\t%s version %s\n\n", PROGRAM_NAME, VERSION);
    fprintf(stderr, "%s [-h] [-a domain]\n", PROGRAM_NAME);
    fprintf(stderr, "-h \t help\n");
    fprintf(stderr, "-a \t domain address.\n");

}

