#include "adc.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    ADCHeader header;
    ADCSample *samples = NULL;

    if (argc != 2) {
        printf("Usage: %s adc_sensor_log.bin\n", argv[0]);
        return 1;
    }

    if (!read_adc_file(argv[1], &header, &samples)) {
        return 1;
    }

    printf("Loaded %u records successfully.\n", header.record_count);

    free(samples);
    return 0;
}