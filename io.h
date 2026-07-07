#ifndef IO_H
#define IO_H

#include "adc.h"

int read_adc_file(const char *filename, ADCHeader *header, ADCSample **samples);

#endif