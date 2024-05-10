#ifndef RTKNAV_RNXPRO_HH_H
#define RTKNAV_RNXPRO_HH_H

#include <stdio.h>
#include <stdlib.h>
#include "nav.h"

#define RNX_MAXTYPE 30
#define RNX_MAXSHFT 10

/*----------code index of rinex files----------*/
typedef struct rnx_CodeIndex_t
{
	unsigned char pn[NAV_FREQYN], ln[NAV_FREQYN], sn[NAV_FREQYN], dn[NAV_FREQYN];
	// index
	unsigned char pindex[NAV_FREQYN][RNX_MAXTYPE];
	unsigned char lindex[NAV_FREQYN][RNX_MAXTYPE];
	unsigned char sindex[NAV_FREQYN][RNX_MAXTYPE];
	unsigned char dindex[NAV_FREQYN][RNX_MAXTYPE];
}rnx_CodeIndex_t;

typedef struct rnx_PhaShift_t
{
	unsigned char Type, SatNum, SysId;
	int SatsF[NAV_MAXPRN];
	nav_F8 Cycle;
}rnx_PhaShift_t;

/*----------observation data struct----------*/
typedef struct rnx_ObsData_t
{
	// observation flag[G|C|R|M] and time flag.
	unsigned char SysType, TimeSys[4];

	// marker serial and name, the end is '\0'.
	unsigned char Markers[61], MarkerNum[21];

	// receiver ant serial and type.
	unsigned char Serials[21], AntType[21];

	// obs type count, type value.
	unsigned char ObsTypeN[NAV_SYSNUM];
	unsigned char ObsTypeV[NAV_SYSNUM][RNX_MAXTYPE];

	int ProIntv, PhaShiftNum;

	// rinex version.
	float Version;
	// position
	double PosXYZ[3], AntUEN[3];

	FILE *fp;

	rnx_PhaShift_t PhaShift[RNX_MAXSHFT];

	// code index of C/P/L/S/D
	rnx_CodeIndex_t CodeIndex[NAV_SYSNUM];

	// one epoch data.
	nav_ObsEpoch_t ObsEpoch, ObsEpoch2;

}rnx_ObsData_t;

#endif