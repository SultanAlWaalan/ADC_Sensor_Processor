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

    *samples = malloc(header->record_count * sizeof(ADCSample));

    if (*samples == NULL) {
        printf("Error: memory allocation failed.\n");
        fclose(file);
        return 0;
    }

    for (uint32_t i = 0; i < header->record_count; i++) {
        ADCSampleFile file_record;

        if (fread(&file_record, sizeof(ADCSampleFile), 1, file) != 1) {
            printf("Error: could not read record %u.\n", i);
            free(*samples);
            *samples = NULL;
            fclose(file);
            return 0;
        }

        ADCSample *current = *samples + i;

        current->timestamp = file_record.timestamp;
        current->channel_id = file_record.channel_id;
        current->raw_value = file_record.raw_value;
        current->temperature = file_record.temperature;
        current->status_flags = file_record.status_flags;
        current->sequence_number = file_record.sequence_number;
        current->voltage = 0.0;
    }

    fclose(file);
    return 1;
}

int write_results_file(const char *filename,
                       const ADCHeader *header,
                       const ChannelStats stats[4],
                       const SequenceGap *gaps,
                       int gap_count) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: could not write results file.\n");
        return 0;
    }

    fprintf(file, "ADC Sensor Log Analysis Report\n");
    fprintf(file, "==============================\n\n");

    fprintf(file, "Header Information\n");
    fprintf(file, "------------------\n");
    fprintf(file, "Magic Number: 0x%X\n", header->magic);
    fprintf(file, "Version: %u\n", header->version);
    fprintf(file, "Channel Count: %u\n", header->channel_count);
    fprintf(file, "Record Count: %u\n", header->record_count);
    fprintf(file, "Sample Rate: %u Hz\n\n", header->sample_rate_hz);

    fprintf(file, "Per-Channel Statistics\n");
    fprintf(file, "----------------------\n\n");

    for (int i = 0; i < 4; i++) {
        fprintf(file, "Channel %d\n", stats[i].channel_id);
        fprintf(file, "Samples: %d\n", stats[i].sample_count);
        fprintf(file, "Mean Voltage: %.6f V\n", stats[i].mean_voltage);
        fprintf(file, "Minimum Voltage: %.6f V\n", stats[i].min_voltage);
        fprintf(file, "Maximum Voltage: %.6f V\n", stats[i].max_voltage);
        fprintf(file, "Standard Deviation: %.6f V\n", stats[i].std_deviation);
        fprintf(file, "Overvoltage Count: %d\n", stats[i].overvoltage_count);
        fprintf(file, "Undervoltage Count: %d\n", stats[i].undervoltage_count);
        fprintf(file, "Sensor Fault Count: %d\n", stats[i].sensor_fault_count);
        fprintf(file, "Out-of-Range Count: %d\n\n", stats[i].out_of_range_count);
    }

    fprintf(file, "Sequence Integrity\n");
    fprintf(file, "------------------\n");

    if (gap_count == 0) {
        fprintf(file, "No sequence gaps detected.\n");
    } else {
        fprintf(file, "Sequence gaps detected: %d\n", gap_count);

        for (int i = 0; i < gap_count; i++) {
            fprintf(file,
                    "Gap %d: previous sequence %u, next sequence %u, missing records %u\n",
                    i + 1,
                    gaps[i].previous_sequence,
                    gaps[i].next_sequence,
                    gaps[i].missing_count);
        }
    }

    fclose(file);
    return 1;
}

int write_fault_report_file(const char *filename,
                            const ADCSample *samples,
                            uint32_t count) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: could not write fault report file.\n");
        return 0;
    }

    fprintf(file, "ADC Fault Report\n");
    fprintf(file, "================\n\n");

    fprintf(file, "This file lists every record where a fault condition was detected.\n");
    fprintf(file, "Fault conditions checked:\n");
    fprintf(file, "- Overvoltage: voltage > 3.0 V\n");
    fprintf(file, "- Undervoltage: voltage < 0.3 V\n");
    fprintf(file, "- Sensor Fault: status_flags bit 0 set\n");
    fprintf(file, "- Out of Range: status_flags bit 1 set\n\n");

    fprintf(file,
            "%-8s %-12s %-8s %-10s %-12s %-12s %-20s\n",
            "Record",
            "Timestamp",
            "Channel",
            "Raw",
            "Voltage(V)",
            "Sequence",
            "Fault Type");

    fprintf(file,
            "%-8s %-12s %-8s %-10s %-12s %-12s %-20s\n",
            "------",
            "---------",
            "-------",
            "---",
            "----------",
            "--------",
            "----------");

    int fault_count = 0;

    for (uint32_t i = 0; i < count; i++) {
        const ADCSample *current = samples + i;

        if (current->voltage > 3.0) {
            fprintf(file,
                    "%-8u %-12.4f %-8u %-10u %-12.6f %-12u %-20s\n",
                    i,
                    current->timestamp,
                    current->channel_id,
                    current->raw_value,
                    current->voltage,
                    current->sequence_number,
                    "Overvoltage");

            fault_count++;
        }

        if (current->voltage < 0.3) {
            fprintf(file,
                    "%-8u %-12.4f %-8u %-10u %-12.6f %-12u %-20s\n",
                    i,
                    current->timestamp,
                    current->channel_id,
                    current->raw_value,
                    current->voltage,
                    current->sequence_number,
                    "Undervoltage");

            fault_count++;
        }

        if (current->status_flags & FLAG_SENSOR_FAULT) {
            fprintf(file,
                    "%-8u %-12.4f %-8u %-10u %-12.6f %-12u %-20s\n",
                    i,
                    current->timestamp,
                    current->channel_id,
                    current->raw_value,
                    current->voltage,
                    current->sequence_number,
                    "Sensor Fault");

            fault_count++;
        }

        if (current->status_flags & FLAG_OUT_OF_RANGE) {
            fprintf(file,
                    "%-8u %-12.4f %-8u %-10u %-12.6f %-12u %-20s\n",
                    i,
                    current->timestamp,
                    current->channel_id,
                    current->raw_value,
                    current->voltage,
                    current->sequence_number,
                    "Out of Range");

            fault_count++;
        }
    }

    fprintf(file, "\nTotal fault entries: %d\n", fault_count);

    fclose(file);
    return 1;
}