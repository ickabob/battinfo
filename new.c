#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc_np.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define BATTERY_STATE_VECTOR "hw.acpi.battery.state\0"
#define BATTERY_STATE BATTERY_STATE_VECTOR
#define BATTERY_LIFE_VECTOR "hw.acpi.battery.life\0"
#define BAT_LIF_VEC	BATTERY_LIFE_VECTOR

static void
usage(void)
{
    fprintf(stderr, "usage: battprobe [-v] [-q]\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
	int c;
	int quiet = 0;
	int verbose = 0;	
	int *mibp = NULL; 	//points to mib data.
	size_t len = 64;	//size of mib.	
	size_t tmp = 0;

	while ((c = getopt(argc, argv, "v")) != -1)
	switch (c) {
	case 'q':
		quiet = 1;
		break;
	case 'v':
		verbose = 1;		
		break;
	default:
		usage();
	}	
	argc -= optind;
	argv += optind;

	if (verbose)
	printf("probing characteristics of %s\n", BATTERY_STATE);
	//accepts an ASCII representation of the name
	//returns the numeric representation in the mib array pointed to by mib
	if (0 > sysctlnametomib(BATTERY_STATE, NULL, &len)) {
		if (! quiet)
		warn("can't find vector %s", BATTERY_STATE);
		return 1;
	} else if (quiet) {
		return 0;
	}

	if (verbose) 
	printf("allocating %d bytes for mib buffer.\n", len * sizeof(int));

	if ((int*)0 > (mibp = calloc(len, sizeof(int))))
	err(1, "out of memory.");

	tmp = len;
	if (tmp != sysctlnametomib(BATTERY_STATE, mibp, &len)) {	
		for (int i = 0; i < len; i++)
		printf("0x%X ", mibp[i]);
		printf("\n");
	return 0;
	}
	return 1;
}
	
