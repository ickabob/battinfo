/*
 *TODO: 
 * 	Use sysctlnametomib to lookup 'hw.acpi.battery.state' and 
 *  'hw.acpi.battery.life' in the "Management Information 
 *   Base" (MIB) used by sysctl. 
 */
#define BSD_SOURCE
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc_np.h>

#define BATTERY_STATE_VECTOR "hw.acpi.battery.state"
#define BAT_ST_VEC BATTERY_STATE_VECTOR
#define BATTERY_LIFE_VECTOR "hw.acpi.battery.life"
#define BAT_LIF_VEC	BATTERY_LIFE_VECTOR

size_t
get_battery_state(const char *vec, int **pbuff){

	size_t	len = 0;	//length of MIB

	if (-1 == sysctlbyname(vec, NULL, &len, NULL, NULL))
		perror("sysctl");
	
	pbuff = calloc(len, sizeof(int));
	if (NULL == (void *)pbuff)
	{
		perror("malloc");
		return -1;
	}

	//the mib information is coppied into mib_buffer.
  	if (-1 == sysctlbyname(vec, *pbuff, &len, NULL, NULL))
	{
		perror("systctl");
		len = -1;
	}
	else {
		// Output info to user on stderr.
		#if DEBUG
		//TODO: Fix this logging routine such that it tells users
		//			size of mib arry (how many integers), and the values.
		log("ret: %d\n bytes read: %d\n, battery state: %d",\
			  ret, len, pbuff);
		#endif
	}	
	return len;
}

int 
main(int argc, char *argv[]){	

	const size_t mib_name_size = 64;//max length for MIB names.

	int		i = 0;					//standard index;
	int		*mib_buffer = NULL;	 	//buffer for MIB values.
	size_t	len = 0; 				//buffer for lenght of MIB buffer.
	char 	*mib_name_buffer = NULL;//buffer for name of MIB vector.

	//allocate and zero out buffer.
	mib_name_buffer = calloc(mib_name_size, sizeof(char)); 

	//set up mib name vector for system probe.
	mib_name_buffer = strncpy(mib_name_buffer, BATTERY_STATE_VECTOR, \
			 				  sizeof(BATTERY_STATE_VECTOR));

	if (-1 == sysctlbyname(mib_name_buffer, NULL, &len, NULL, NULL))
	{
		perror("sysctl");
		exit(-1);
	}

	mib_buffer = calloc(len * sizeof(int));
	if (NULL == mib_buffer)
	{
		perror("malloc");
		exit(-1);
	}

	//get battery state from acpi
	len = sysctlbyname(mib_name_buffer, &mib_buffer[0]);
	if (-1 == len || NULL == mib_buffer)
	{
		perror("get_battery_state");
		exit(-1);
	}	 
	
	printf(BATTERY_STATE_VECTOR);
	printf(" MIB:\t%zd", len);
	for (i = 0; i < len; i++)
		printf("MIB[%zd]:\t%d", i, mib_buffer[i]);
	
	exit(0);
	//when running on battery, get battery life from acpi.
	//check previous MIB return value against known constant.
}
