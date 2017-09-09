#ifndef CURRENT_SOURCE_H
#define CURRENT_SOURCE_H

#include <stdbool.h>
#include <inttypes.h>

enum current_ranges {
    RANGE_20MA,
    RANGE_100MA,
    RANGE_1A,
    NUM_RANGES
};

void start_current_source(void);
void stop_current_source(void);

// Step the range forward, cycling around at the end.
void step_current_range(void);

// Set the range to the given setting
void set_current_range(enum current_ranges range);

#endif // CURRENT_SOURCE_H
