#ifndef _NCT75_H
#define _NCT75_H

typedef enum {
    NCT_RIGHT = 0,
    NCT_TOP = 1,
    NCT_LEFT = 2,
} nct_type_t;

typedef struct {
    int top;
    int left;
    int right;
    int is_rescuing;
    int is_overheat;
} sys_temperature_t;

extern sys_temperature_t g_temperature;

int nct_read_temperature(nct_type_t type);

#endif
