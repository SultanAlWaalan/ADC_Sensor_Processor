#include "adc.h"
#include "stats.h"
#include <stdlib.h>

void convert_all_voltages(ADCSample *samples, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        ADCSample *current = samples + i;
        current->voltage = (current->raw_value / ADC_MAX_RAW) * ADC_VREF;
    }
}

void compute_channel_statistics(const ADCSample *samples,
                                uint32_t count,
                                ChannelStats stats[4]) {
    for (int ch = 0; ch < 4; ch++) {
        stats[ch].channel_id = ch;
        stats[ch].sample_count = 0;
        stats[ch].mean_voltage = 0.0;
        stats[ch].min_voltage = 0.0;
        stats[ch].max_voltage = 0.0;
        stats[ch].std_deviation = 0.0;
        stats[ch].overvoltage_count = 0;
        stats[ch].undervoltage_count = 0;
        stats[ch].sensor_fault_count = 0;
        stats[ch].out_of_range_count = 0;
    }

    int channel_counts[4] = {0, 0, 0, 0};

    for (uint32_t i = 0; i < count; i++) {
        const ADCSample *current = samples + i;

        if (current->channel_id < 4) {
            channel_counts[current->channel_id]++;
        }
    }

    double *channel_values[4];

    for (int ch = 0; ch < 4; ch++) {
        channel_values[ch] = malloc(channel_counts[ch] * sizeof(double));

        if (channel_values[ch] == NULL) {
            stats[ch].sample_count = 0;
        } else {
            stats[ch].sample_count = channel_counts[ch];
        }

        channel_counts[ch] = 0;
    }

    for (uint32_t i = 0; i < count; i++) {
        const ADCSample *current = samples + i;

        if (current->channel_id < 4) {
            int ch = current->channel_id;

            if (channel_values[ch] != NULL) {
                channel_values[ch][channel_counts[ch]] = current->voltage;
                channel_counts[ch]++;
            }

            if (current->voltage > 3.0) {
                stats[ch].overvoltage_count++;
            }

            if (current->voltage < 0.3) {
                stats[ch].undervoltage_count++;
            }

            if (current->status_flags & FLAG_SENSOR_FAULT) {
                stats[ch].sensor_fault_count++;
            }

            if (current->status_flags & FLAG_OUT_OF_RANGE) {
                stats[ch].out_of_range_count++;
            }
        }
    }

    for (int ch = 0; ch < 4; ch++) {
        if (channel_values[ch] != NULL && stats[ch].sample_count > 0) {
            stats[ch].mean_voltage =
                    calculate_mean(channel_values[ch], stats[ch].sample_count);

            stats[ch].min_voltage =
                    calculate_min(channel_values[ch], stats[ch].sample_count);

            stats[ch].max_voltage =
                    calculate_max(channel_values[ch], stats[ch].sample_count);

            stats[ch].std_deviation =
                    calculate_standard_deviation(channel_values[ch],
                                                 stats[ch].sample_count,
                                                 stats[ch].mean_voltage);
        }

        free(channel_values[ch]);
    }
}

int detect_sequence_gaps(const ADCSample *samples,
                         uint32_t count,
                         SequenceGap *gaps,
                         int max_gaps) {
    int gap_count = 0;

    for (uint32_t i = 0; i + 1 < count; i++) {
        const ADCSample *current = samples + i;
        const ADCSample *next = samples + i + 1;

        if (next->sequence_number != current->sequence_number + 1) {
            if (gap_count < max_gaps) {
                gaps[gap_count].previous_sequence = current->sequence_number;
                gaps[gap_count].next_sequence = next->sequence_number;
                gaps[gap_count].missing_count =
                        next->sequence_number - current->sequence_number - 1;
            }

            gap_count++;
        }
    }

    return gap_count;
}