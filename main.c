#include "adc.h"
#include "io.h"
#include <stdio.h>

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

    printf("Header read successfully.\n");
    printf("Records: %u\n", header.record_count);

    return 0;
}