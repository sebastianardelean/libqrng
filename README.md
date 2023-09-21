```{=org}
#+STARTUP: inlineimages
```
# LibQrng

## Introduction

## Software Architecture

![This is the caption for the next figure link (or
table)](./images/libqrng_initialization.png "libqrng_initialization")

![This is the caption for the next figure link (or
table)](./images/libqrng_random_stream.png "libqrng_random_stream")

![This is the caption for the next figure link (or
table)](./images/libqrng_double_value.png "libqrng_double_value")

![This is the caption for the next figure link (or
table)](./images/libqrng_int64.png "libqrng_int64")

![This is the caption for the next figure link (or
table)](./images/libqrng_cleanup.png "libqrng_cleanup")

## Build

## Installation

## Examples

                    qrand version 1.0.28-25.05.2023

    qrand [-h] [-a domain] [-s no of samples] [-m min double value] [-M max double value] [-i min int value] [-I max int value] [-t type] [-f stream]
    -h       help
    -a       domain address. [Default: random.cs.upt.ro]
    -s       number of samples. [Default 1]
    -m       min value double. [Default 0.0]
    -M       max value double. [Default 1.0]
    -i       min value int64. [Default 0]
    -I       max value int64. [Default 1]
    -t       type.
    -f       stream.

    ================================
    Possible values for t:
            0 -> 32-bit integer
            1 -> 64-bit integer
            2 -> double value
            3 -> float value
            4 -> stream of bytes
            5 -> device performance
            6 -> firmware info
            7 -> system info
    ```
