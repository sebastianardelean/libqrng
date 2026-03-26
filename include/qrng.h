/****************************************************************************
 * libqrng - library for interacting with IDQ's Quantis Appliance           *
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
 * @file qrng.h
 * @author Sebastian Mihai Ardelean <sebastian.ardelean@cs.upt.ro>
 * @date 24 May 2023
 * @brief API for interacting with IDQ's Quantis Appliance
 */

#ifndef QRNG_H
#define QRNG_H


#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Initialization function
 * This function must be called to initialize libcurl and to configure the URL addresses.
 * @param device_domain_address domain address of the IDQ Quantis Appliance device.
 * @return Function returns 0 on SUCCESS, -1 if @curl_global_init@ fails, -2 if the libcurl handle cannot be initialized, and -3 if the @device_domain_address@ is NULL.
 * @note On failure, the function performs clean-up.
 */
int qrng_open(const char *device_domain_address);

/**
 * @brief Generate a stream of random bytes.
 * @param stream FILE pointer in which the bytes are stored. @stdout@ or @stderr@ can be used.
 * @param size the number of bytes to request.
 * @return Function returns 0 on SUCCESS and -1 if libcurl cannot perform the request.
 */  
int qrng_random_stream(FILE *stream, size_t size);

/**
 * @brief Generate random @double@ values.
 * This function generates random @double@ values in the specified interval.
 * @param min interval minimum value.
 * @param max interval maximum value.
 * @param samples number of values to generate.
 * @param buffer array of type @double@ in which the values will be stored.
 * @return This function returns 0 on SUCCESS, -1 if the internal buffer cannot be initialized, or if libcurl cannot perform the request.
 */  
int qrng_random_double(double min, double max, size_t samples, double *buffer);

/**
 * @brief Generate random @float@ values.
 * This function generates random @float@ values in the specified interval.
 * @param min interval minimum value.
 * @param max interval maximum value.
 * @param samples number of values to generate.
 * @param buffer array of type @float@ in which the values will be stored.
 * @return This function returns 0 on SUCCESS, -1 if the internal buffer cannot be initialized, or if libcurl cannot perform the request.
 */    
int qrng_random_float(float min, float max, size_t samples, float *buffer);


/**
 * @brief Generate random @int16@ values.
 * This function generates random @int16@ values in the specified interval.
 * @param min interval minimum value.
 * @param max interval maximum value.
 * @param samples number of values to generate.
 * @param buffer array of type @int16@ in which the values will be stored.
 * @return This function returns 0 on SUCCESS, -1 if the internal buffer cannot be initialized, or if libcurl cannot perform the request.
 */    
int qrng_random_int16(int16_t min, int16_t max, size_t samples, int16_t *buffer);



/**
 * @brief Generate random @int8@ values.
 * This function generates random @int8@ values.
 * @param samples number of values to generate.
 * @param buffer array of type @uint8@ in which the values will be stored.
 * @return This function returns 0 on SUCCESS, -1 if the internal buffer cannot be initialized, or if libcurl cannot perform the request.
 */    
int qrng_random_bytes(size_t samples, uint8_t *buffer);
/**
 * @brief Generate random @int32@ values.
 * This function generates random @int32@ values in the specified interval.
 * @param min interval minimum value.
 * @param max interval maximum value.
 * @param samples number of values to generate.
 * @param buffer array of type @int32@ in which the values will be stored.
 * @return This function returns 0 on SUCCESS, -1 if the internal buffer cannot be initialized, or if libcurl cannot perform the request.
 */    
int qrng_random_int32(int32_t min, int32_t max, size_t samples, int32_t *buffer);

/**
 * @brief Requests the IDQ's Quantis Appliance firmware version. 
 * @param buffer pointer to FILE or @stdout@ to which data will be sent.
 * @return Function returns 0 on SUCCESS and -1 if libcurl cannot perform the request.
 */    
int qrng_firmware_info(void *buffer);

/**
 * @brief Requests the IDQ's Quantis Appliance system info. 
 * @param buffer pointer to FILE or @stdout@ to which data will be sent.
 * @return Function returns 0 on SUCCESS and -1 if libcurl cannot perform the request.
 */  
int qrng_system_info(void *buffer);

/**
 * @brief Close function
 * This function must be called for clean-up. It performs libcurl clean-up.
 * @note If @qrng_open@ fails, is not mandatory to call this function.
 */
void qrng_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* QRNG_H */
