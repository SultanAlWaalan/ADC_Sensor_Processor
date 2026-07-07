#ifndef IO_H
#define IO_H

#include "adc.h"

int read_adc_file(const char *filename, ADCHeader *header, ADCSample **samples);

int write_results_file(const char *filename,
                       const ADCHeader *header,
                       const ChannelStats stats[4],
                       const SequenceGap *gaps,
                       int gap_count);

int write_fault_report_file(const char *filename,
                            const ADCSample *samples,
                            uint32_t count);
#endif