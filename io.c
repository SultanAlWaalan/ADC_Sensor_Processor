#include "io.h"
#include <stdio.h>
#include <stdlib.h>

int read_adc_file(const char *filename, ADCHeader *header, ADCSample **samples) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Error: could not open input file.\n");
        return 0;
    }

    if (fread(header, sizeof(ADCHeader), 1, file) != 1) {
        printf("Error: could not read file header.\n");
        fclose(file);
        return 0;
    }

    if (header->magic != ADC_MAGIC) {
        printf("Error: invalid magic number.\n");
        fclose(file);
        return 0;
    }

    if (header->version != ADC_EXPECTED_VERSION) {
        printf("Error: unsupported file version.\n");
        fclose(file);
        return 0;
    }

    if (header->channel_count != ADC_EXPECTED_CHANNELS) {
        printf("Error: unexpected channel count.\n");
        fclose(file);
        return 0;
    }

    if (header->sample_rate_hz != ADC_EXPECTED_SAMPLE_RATE) {
        printf("Error: unexpected sample rate.\n");
        fclose(file);
        return 0;
    }

    if (header->record_count == 0) {
        printf("Error: record count is zero.\n");
        fclose(file);
        return 0;
    }

    *samples = NULL;

    fclose(file);
    return 1;
}