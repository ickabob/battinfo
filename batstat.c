#define _BSD_SOURCE
#define C11
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h> //C99
#include <sys/sysctl.h>

#include "batstat.h"

extern uint8_t verbose;
extern uint8_t quiet;

//forward declaration for smprintf utility.
char * smprintf(char *fmt, ...);

static size_t 
init_sysctl(const char * mib_name, intptr_t **mibvector_p)
{
	size_t len = 0;
	intptr_t *buff = NULL;

	//lookup storage required to hold translation of mib_name
	if (verbose)
		printf("translating %s to numerical mib vector.", mib_name);
	if ( 0 > sysctlnametomib(mib_name, NULL, &len))
	{
		perror("sysctlnametomib");
		return -1;	
	}
	//allocate storage for mib
	if ( NULL == (buff = (intptr_t*)calloc(sizeof(intptr_t), len)))
	{
		perror("calloc");
		return -1;
	}
	//change external pointer to reference beginning of newly allocated buffer.
	*mibvector_p = buff;
	//request current value into mibvector_p
	if ( 0 > sysctlnametomib(mib_name, *mibvector_p, &len))
	{
		perror("sysctlnametomib");
		return -1;
	}
	return len;
}


powerSource
statPwrSrc(int8_t *percent_charge)
{
	static uint8_t init_flag;
	
	/* MIB vectors */
	static intptr_t *acline_mibp;
	static intptr_t *batstate_mibp;
	static intptr_t *batlife_mibp;
	size_t sz_aclinemib;
	size_t sz_bstatemib;
	size_t sz_blifemib;


	if (!init_flag)
 	{//sysctllnametomib runs ~3 times slower than sysctl sys call. 
	 //initialize the mib names first run through for subsequent sysctl calls.
		sz_aclinemib = init_sysctl(AC_ACLINE_OID, &acline_mibp);
		sz_bstatemib = init_sysctl(BATTERY_STATE_OID, &batstate_mibp);
		sz_blifemib = init_sysctl(BATTERY_LIFE_OID, &batlife_mibp);

		if (sz_bstatemib && sz_blifemib)
			init_flag = true;
	}

	if (!init_flag)
		return UNKNOWN_ERROR;	

	intptr_t acline 	= 0;
	intptr_t batstate 	= 0;
	intptr_t batlife 	= 0;
	size_t sz_acline	= sizeof(acline);
	size_t sz_batstate	= sizeof(batstate);
	size_t sz_batlife 	= sizeof(batlife);

	//battery state
	if (0 > sysctl(batstate_mibp, sz_bstatemib, &batstate, &sz_batstate,\
						   	NULL, 0))
	{ 
		perror(BATTERY_STATE_OID); 
		*percent_charge = -1;
		return UNKNOWN_SRC;
	}
	switch (batstate) {
	case _STATE_CHARGING: //plugged in to adapter.		 
		if (0 > sysctl(batlife_mibp, sz_blifemib, &batlife,\
							   	&sz_batlife, NULL, 0))
		{
			perror(BATTERY_LIFE_OID);
			*percent_charge = -1;
			return ACADAPTER;
		} 
		*percent_charge = batlife;
		return BATTERY;
	case _STATE_DISCHARGING: //on battery power.
		if (0 > sysctl(batlife_mibp, sz_blifemib, &batlife,\
							   	&sz_batlife, NULL, 0))
		{
			perror(BATTERY_LIFE_OID);
			*percent_charge = -1;
			return BATTERY;
		}
		*percent_charge = batlife;
		return BATTERY;
	default:
		if (0 > sysctl(acline_mibp, sz_aclinemib, &acline, &sz_acline, NULL, 0))
		{
			perror(AC_ACLINE_OID);
			return UNKNOWN_SRC;
		}	
		//plugged into adapter with battery out.
		*percent_charge = -1;
		return ACADAPTER;
	}	
}
