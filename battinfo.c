
#define _BSD_SOURCE
#define C11

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include "batstat.h"

uint8_t quiet = false;
uint8_t verbose = false;

//forward declaration for smprintf utility.
char * smprintf(char *fmt, ...);

static inline void 
print_usage(void)
{
	const char * usage = "usage: [-v] [-q]\n\0";

	fprintf(stderr, "%s", usage);
	exit(1);	
}

int32_t
main(int32_t argc, char *argv[])
{
	char * status_buff = NULL;

	int32_t c = 0;

	while (-1 != (c = getopt(argc, argv, "v")))
		switch (c) {
		case 'q':
			quiet = true;
			break;
		case 'v':
			verbose = true;
			break;
		default:
			print_usage();
		}
	argc -= optind;
	argv += optind;

	int8_t percent_charge;
	powerSource pwrsrc = statPwrSrc(&percent_charge);
	switch (pwrsrc){
			case ACADAPTER:
				if (0 > percent_charge)
					status_buff = smprintf("AC: [ - ]");
				else
					status_buff = smprintf("AC: [%d]", percent_charge);
			break;
			case BATTERY:
				status_buff = smprintf("BAT: [%d]", percent_charge);			
			break;
			default:
				perror("unknown power source");
				exit(1);
	}
	printf("%s", status_buff);	
	free(status_buff);	
	exit(0);
}

