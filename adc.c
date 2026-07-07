#include "adc.h"

void convert_all_voltages(ADCSample *samples, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        ADCSample *current = samples + i;
        current->voltage = (current->raw_value / ADC_MAX_RAW) * ADC_VREF;
    }
}