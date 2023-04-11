#pragma once

typedef enum {
    HW_REV_UNKNOWN = 0,
    HW_REV_1,
    HW_REV_2
} hw_revision_t;

hw_revision_t hwRevision();
int read_voltage(void);
