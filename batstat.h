#define C11
#pragma once

#include <stdint.h>

/* Battery States */
#define _STATE_CHARGING		1
#define _STATE_DISCHARGING	2
#define _STATE_UNKNOWN		7

typedef enum powerSource{
		ACADAPTER = 1,
		BATTERY,
		UNKNOWN_SRC,
		UNKNOWN_ERROR
} powerSource;

/* Sysctl OID's */
#define AC_ACLINE_OID "hw.acpi.acline\0"
#define BATTERY_STATE_OID "hw.acpi.battery.state\0"
#define BATTERY_LIFE_OID "hw.acpi.battery.life\0"

powerSource
statPwrSrc(int8_t *percent_charge);

