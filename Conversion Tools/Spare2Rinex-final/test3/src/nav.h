#ifndef RTKNAV_HH_H
#define RTKNAV_HH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
	//#ifdef RTK_DLLEXPORT
	//#define NAV_API _declspec(dllexport)
	//#else
	//#define NAV_API _declspec(dllimport)
	//#endif
#define NAV_MAX(x, y)  ((x) < (y) ? (y) : (x))

#define NAV_API

#define NAV_FREQYN   3
#define NAV_MAXSAT  50
/*---------- navigation satellite system -------*/
	enum nav_systems {
		NAV_GPS, NAV_GLO, NAV_BDS, NAV_GAL, NAV_SYSNUM
	};

#define NAV_GPSLEAPSTART 19
#define NAV_F_WGS84     (1./298.257223563)
#define NAV_R_WGS84         6378137.0E+00
#define NAV_R_PZ_90         6378136.0E+00
#define NAV_R_J2            1.0826257E-03
#define NAV_R_MU            3.9860044E+14 
#define NAV_C_LIGHT       299792458.0E+00
#define NAV_E_GM            3986005.0E+08
#define NAV_G_OMEGA      7.2921151467E-05
#define NAV_R_OMEGA          7.292115E-05
#define NAV_E_OMEGA      7.2921151467E-05
#define NAV_C_OMEGA          7.292115E-05
#define NAV_G_FREQ1      1575420000.0E+00
#define NAV_G_FREQ2      1227600000.0E+00
#define NAV_G_FREQ5      1176450000.0E+00
#define NAV_R_FREQ1      1602000000.0E+00
#define NAV_R_FREQ2      1246000000.0E+00
#define NAV_R_FREQ3      1202025000.0E+00
#define NAV_R_FREQ1a     1600995000.0E+00
#define NAV_R_FREQ2a     1248060000.0E+00
#define NAV_R_STEP1          562500.0E+00
#define NAV_R_STEP2          437500.0E+00
#define NAV_E_FREQ1      1575420000.0E+00
#define NAV_E_FREQ5      1191795000.0E+00
#define NAV_E_FREQ5a     1176450000.0E+00
#define NAV_E_FREQ5b     1207140000.0E+00
#define NAV_E_FREQ6      1278750000.0E+00
#define NAV_C_FREQ1_3    1575420000.0E+00
#define NAV_C_FREQ1_2    1561098000.0E+00
#define NAV_C_FREQ2a     1176450000.0E+00
#define NAV_C_FREQ2b     1207140000.0E+00
#define NAV_C_FREQ2      1191795000.0E+00
#define NAV_C_FREQ3      1268520000.0E+00
 
#define NAV_ENABLE_GPS
#define NAV_ENABLE_GLO
#define NAV_ENABLE_GAL
#define NAV_ENABLE_BDS

	/*----------satellite id list of GPS----------*/
#ifdef  NAV_ENABLE_GPS
#define NAV_MAX_GPS_NUM  32
#define NAV_MIN_GPS_PRN   1
#define NAV_MAX_GPS_PRN  (NAV_MIN_GPS_PRN+NAV_MAX_GPS_NUM-1)
#else
#define NAV_MAX_GPS_NUM   0
#define NAV_MIN_GPS_PRN   0
#define NAV_MAX_GPS_PRN   0
#define NAV_MAX_GPS_OBS   0
#endif
/*----------satellite id list of GLO----------*/
#ifdef  NAV_ENABLE_GLO
#define NAV_MAX_GLO_NUM  26
#define NAV_MIN_GLO_PRN  (NAV_MAX_GPS_NUM+1)
#define NAV_MAX_GLO_PRN  (NAV_MIN_GLO_PRN+NAV_MAX_GLO_NUM-1)
#else
#define NAV_MAX_GLO_NUM   0
#define NAV_MIN_GLO_PRN   0
#define NAV_MAX_GLO_PRN   0
#endif
/*----------satellite id list of BDS----------*/
#ifdef  NAV_ENABLE_BDS
#define NAV_MAX_BDS_NUM  64
#define NAV_MIN_BDS_PRN  (NAV_MAX_GPS_NUM+NAV_MAX_GLO_NUM+1)
#define NAV_MAX_BDS_PRN  (NAV_MIN_BDS_PRN+NAV_MAX_BDS_NUM-1)
#else
#define NAV_MAX_BDS_NUM  0
#define NAV_MIN_BDS_PRN  0
#define NAV_MAX_BDS_PRN  0
#endif
/*----------satellite id list of GAL----------*/
#ifdef  NAV_ENABLE_GAL
#define NAV_MAX_GAL_NUM  35
#define NAV_MIN_GAL_PRN  (NAV_MAX_GPS_NUM+NAV_MAX_GLO_NUM+NAV_MAX_BDS_NUM+1)
#define NAV_MAX_GAL_PRN  (NAV_MIN_GAL_PRN+NAV_MAX_GAL_NUM-1)
#else
#define NAV_MAX_GAL_NUM  0
#define NAV_MIN_GAL_PRN  0
#define NAV_MAX_GAL_PRN  0
#endif

/*----------total satellite id number---------*/
#define NAV_MAXPRN  (NAV_MAX_GPS_NUM+NAV_MAX_GLO_NUM+NAV_MAX_GAL_NUM+NAV_MAX_BDS_NUM+1)

	typedef   signed  char nav_I1;
	typedef unsigned  char nav_U1;
	typedef   signed short nav_I2;
	typedef unsigned short nav_U2;
	typedef   signed   int nav_I4;
	typedef unsigned   int nav_U4;
	typedef long long  int nav_I8;
	typedef          float nav_F4;
	typedef         double nav_F8;

	#define NAV_LOG_OFF        0
	#define NAV_LOG_ERROR      1
	#define NAV_LOG_WARN       2
	#define NAV_LOG_TRACE      3
	#define NAV_LOG_INFO       4
	#define NAV_LOG_DEBUG      5
	#define NAV_LOG_LEVELN     6

	/*---------- GPST struct -------*/
	typedef struct nav_GPSt_t
	{
		nav_I4 Week;
		nav_F8 Tow;

	} nav_GPSt_t;

	typedef struct nav_Date_t
	{
		nav_I4 Year;
		nav_U1 Month, Day, Hour, Minute;
		nav_F8 Sec, Jd;

	}nav_Date_t;

	/*---------- satellite observation struct -------*/
	typedef struct nav_ObsSat_t
	{
		// state, cycle slip flags, SNR.
		nav_I1 State;
		// satellite id.
		nav_I4 SatNo;
		// phase type and code range type.
		nav_U1 Type[NAV_FREQYN], Slip[NAV_FREQYN], SNR[NAV_FREQYN], LLI[NAV_FREQYN];
		// observation value.
		nav_F4 D[NAV_FREQYN];
		nav_F8 L[NAV_FREQYN], P[NAV_FREQYN], vL[NAV_FREQYN], vP[NAV_FREQYN], vD[NAV_FREQYN];

	} nav_ObsSat_t;

	typedef struct nav_ObsEpoch_t
	{
		// satellite number, epoch flags.
		nav_I1 SatNum;

		// observation time(GPST)
		nav_GPSt_t t;

		// obs data of all satellites.
		nav_ObsSat_t SatData[NAV_MAXSAT];

	} nav_ObsEpoch_t;


		
#ifdef __cplusplus
}
#endif

#endif
