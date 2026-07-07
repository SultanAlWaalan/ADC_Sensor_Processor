#include "stats.h"
#include <math.h>

double calculate_mean(const double *values, int count) {
    double sum = 0.0;

    for (int i = 0; i < count; i++) {
        sum += *(values + i);
    }

    return sum / count;
}

double calculate_min(const double *values, int count) {
    double min = *values;

    for (int i = 1; i < count; i++) {
        if (*(values + i) < min) {
            min = *(values + i);
        }
    }

    return min;
}

double calculate_max(const double *values, int count) {
    double max = *values;

    for (int i = 1; i < count; i++) {
        if (*(values + i) > max) {
            max = *(values + i);
        }
    }

    return max;
}

double calculate_standard_deviation(const double *values, int count, double mean) {
    double sum_squared_difference = 0.0;

    for (int i = 0; i < count; i++) {
        double difference = *(values + i) - mean;
        sum_squared_difference += difference * difference;
    }

    return sqrt(sum_squared_difference / count);
}