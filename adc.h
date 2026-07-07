#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_MAGIC 0xADC1BEEF
#define ADC_EXPECTED_VERSION 1
#define ADC_EXPECTED_CHANNELS 4
#define ADC_EXPECTED_SAMPLE_RATE 1000

#define ADC_VREF 3.3
#define ADC_MAX_RAW 4095.0

#define FLAG_SENSOR_FAULT 0x01
#define FLAG_OUT_OF_RANGE 0x02

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate_hz;
    uint8_t reserved[8];
} ADCHeader;

typedef struct __attribute__((packed)) {
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    int16_t temperature;
    uint8_t status_flags;
    uint32_t sequence_number;
    uint8_t reserved[2];
} ADCSampleFile;

#endif