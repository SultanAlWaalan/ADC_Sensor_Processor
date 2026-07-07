#include "adc.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    ADCHeader header;
    ADCSample *samples = NULL;
    ChannelStats stats[4];
    SequenceGap gaps[20];

    if (argc != 2) {
        printf("Usage: %s adc_sensor_log.bin\n", argv[0]);
        return 1;
    }

    if (!read_adc_file(argv[1], &header, &samples)) {
        return 1;
    }

    convert_all_voltages(samples, header.record_count);

    compute_channel_statistics(samples, header.record_count, stats);

    int gap_count = detect_sequence_gaps(samples, header.record_count, gaps, 20);

    if (!write_results_file("results.txt", &header, stats, gaps, gap_count)) {
        free(samples);
        return 1;
    }

    if (!write_fault_report_file("fault_report.txt", samples, header.record_count)) {
        free(samples);
        return 1;
    }

    printf("Analysis complete.\n");
    printf("Summary written to results.txt\n");
    printf("Fault report written to fault_report.txt\n");

    free(samples);
    return 0;
}