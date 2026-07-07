#ifndef STATS_H
#define STATS_H

double calculate_mean(const double *values, int count);
double calculate_min(const double *values, int count);
double calculate_max(const double *values, int count);
double calculate_standard_deviation(const double *values, int count, double mean);

#endif