#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc_np.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define BATTERY_OID "hw.acpi.battery.state\0"
#define BATTERY_STATE_VECTOR "hw.acpi.battery.state\0"
#define BATTERY_STATE BATTERY_STATE_VECTOR
#define BATTERY_LIFE_VECTOR "hw.acpi.battery.life\0"
#define BAT_LIF_VEC	BATTERY_LIFE_VECTOR

//standard print usage.
static void
usage(void)
{
    fprintf(stderr, "usage: battprobe [-v] [-q]\n");
    exit(1);
}

//mib - mib buffer should hold mib data.
//len - lenght of mib buffer.
static inline void
print_translated_mib(const int *mib, const int len)
{
	for (int i = 0; i < len; i++)
		printf("0x%X ", mib[i]);
	printf("\n");
	return;
}

static inline void
print_buffer(const char *buff, const int len)
{
	for (int i = 0; i < len; i++)
		printf("0x%hhX ", buff[i]);
	printf("\n");
	return;
}

int
main(int argc, char *argv[])
{
	int c;
	int quiet = 0;
	int verbose = 0;	
	int *mibp = NULL; 	//mib style name.
	char *buff = NULL;
	size_t len_name = 0;//size of mib style name.
	size_t len = 0;	
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
		printf("looking up integer name vector for: %s\n", BATTERY_OID);
	//probe mib structure size, storing value @ len.
	if (0 > sysctlnametomib(BATTERY_OID, NULL, &len_name)) {
		if (! quiet)
			warn("can't find vector %s", BATTERY_OID);
		return 1;
	} else if (quiet) {
		return 0;
	}

	if (verbose) 
		printf("allocating %d bytes for mib buffer.\n", len_name *\
					   	sizeof(int));
	//allocate requisite space for integer mib name.
	if ((int*)0 > (mibp = calloc(len_name, sizeof(int))))
		err(1, "out of memory.");

	tmp = len_name;
	//translate ASCII to mib.
	if (0 > sysctlnametomib(BATTERY_OID, mibp, &len_name)) 
		err(1,"failed to translate %s", BATTERY_OID);
	else if (tmp != len_name) {
		warn("size of mib buffer may have changed since probe.");
	}
	printf("%s translated to : ", BATTERY_OID);
	print_translated_mib(mibp, len_name);

	//TODO: check sysctl.h for proper types.
	//Currently allocating a buffer of B bytes where B is the 
	//length returned from sysctl after a probe.
	//
	
	//Probe length.
	if (0 > sysctl(mibp, len_name, NULL, &len, NULL, 0))
		err(1,"failed to retreive %s", BATTERY_OID);	
	printf("mib structure length: %d\n", len);
	//allocate buffer of len bytes.
	if ((char*)0 > (buff = calloc(len, sizeof(char))))
		err(1, "out of memory");
	//request data be copied into buffer.
	if (0 > sysctl(mibp, len_name, buff, &len, NULL, 0))
		err(1, "failed to retreive %s", BATTERY_OID);	
	else if (verbose) {
		printf("sysctl copied %d bytes.\n", len);
	}
	print_buffer(buff, len);		
	exit(0);
	



}
	
