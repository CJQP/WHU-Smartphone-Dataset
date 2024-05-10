#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "nav.h"
#include "nav_dll.h"
#include "ccjson.h"
#include "nav_rnx.h"
#include "nav_log.h"


#define MAX_WORD_NUM 35
#define MAX_WORD_STR 100
#define MAX_LINE_LEN 1024

#define ROUND(x)      (floor((x) + 0.5))
#define LIGHTSPEED     2.99792458e8
#define SYNCTIMETOL   (1.0e-6)

#define GPS_MIN_PRN 1
#define GPS_MAX_PRN 35
#define GLO_MIN_PRN 36
#define GLO_MAX_PRN 65
#define GAL_MIN_PRN 66
#define GAL_MAX_PRN 100
#define BDS_MIN_PRN 101
#define BDS_MAX_PRN 145

#define RAW_NFREQ   3                   
#define RAW_MAXSAT 64


typedef struct raw_date_t
{
	int year;
	int month, day, hour, minute;
	double sec;
}raw_date_t;

typedef struct raw_satd_t{

	unsigned char prn;            /* prn */
	unsigned char SNR[RAW_NFREQ];     /* signal strength (dBHz) */
	unsigned char LLI[RAW_NFREQ];     /* loss of lock indicator */
	unsigned char type[RAW_NFREQ][4]; /* code indicator (eg:L1C) */
	double L[RAW_NFREQ], vL[RAW_NFREQ];   /* observation data carrier-phase (cycle) */
	double P[RAW_NFREQ], vP[RAW_NFREQ];   /* observation data pseudorange (m) */
	float  D[RAW_NFREQ], vD[RAW_NFREQ];   /* observation data doppler frequency (Hz) */
}raw_satd_t;

typedef struct raw_obsd_t{

	int satnum;                   
	raw_date_t t;                     
	raw_satd_t d[RAW_MAXSAT];           

}raw_obsd_t;

typedef struct gnssLogOutPos_t
{
	char outfile_pos[1024], outfile_nmea[1024];
	FILE *fp_pos, *fp_nmea;
	int InitFlag_pos;
	nav_Date_t t;
	double rovpos[3];
}gnssLogOutPos_t;


typedef struct gnssRnx_t
{
	int header_flag;
	FILE *fp;
	raw_obsd_t obsd;
	nav_ObsEpoch_t ObsData;
}gnssRnx_t;


static const int gnsslog_time_Months[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

typedef struct leapseconds_t
{
	int day;
	int month;
	int year;
	int taicount;
}leapseconds_t;

static int g_nav_MinSatNo[NAV_SYSNUM] = {
	NAV_MIN_GPS_PRN, NAV_MIN_GLO_PRN, NAV_MIN_BDS_PRN, NAV_MIN_GAL_PRN
};
static int g_nav_MaxSatNo[NAV_SYSNUM] = {
	NAV_MAX_GPS_PRN, NAV_MAX_GLO_PRN, NAV_MAX_BDS_PRN, NAV_MAX_GAL_PRN
};

int nav_sys_GetSysId(char SysFlag)
{
	if (SysFlag == 'G') return NAV_GPS;
	else if (SysFlag == 'R') return NAV_GLO;
	else if (SysFlag == 'E') return NAV_GAL;
	else if (SysFlag == 'C') return NAV_BDS;
	else                     return      -1;
}


char nav_sys_GetSysFlag(int SysID)
{
	if (SysID == NAV_GPS) return 'G';
	else if (SysID == NAV_GLO) return 'R';
	else if (SysID == NAV_GAL) return 'E';
	else if (SysID == NAV_BDS) return 'C';
	else                       return  -1;
}

static const int nav_Months[13] = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/*get total day number of year.*/
static int nav_time_GetDayNumOfYear(int year)
{
	return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 366 : 365;
}

/*get total day number of month.*/
static int nav_time_GetDayOfMonth(int year, int month)
{
	if (month == 2)
		return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) ? 29 : 28;
	else
		return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

double nav_time_Date2JD(nav_Date_t *d)
{
	int year = d->Year, month = d->Month;

	double mhour = d->Hour + d->Minute / 60.0 + d->Sec / 3600.0;

	if (month <= 2) {
		year -= 1;  month += 12;
	}
	d->Jd = (int)(365.25*year) + (int)(30.6001*(month + 1)) + d->Day
		+ mhour / 24.0 + 1720981.5;

	return d->Jd;
}

static int nav_time_IsLongYear(int year, int month)
{
	if (!(year % 4) && (!(year % 400) || (year % 100))) {
		if (!month || month == 2) return 1;
	}
	return 0;
}


/*convert week and secs to common date(GPS).*/
void nav_time_GPStToDate(nav_GPSt_t *c, nav_Date_t *d)
{
	int tow = (int)(c->Tow);
	double utow = c->Tow - tow;

	nav_U4 i, k, doy, j;

	j = c->Week*(7 * 24 * 60 * 60) + tow + 5 * 24 * 60 * 60;

	for (i = 1980; j >= (k = (365 + nav_time_IsLongYear(i, 0)) * 24 * 60 * 60); ++i) j -= k;

	d->Year = i;
	doy = 1 + (j / (24 * 60 * 60));
	j %= (24 * 60 * 60);
	d->Hour = j / (60 * 60);
	j %= (60 * 60);
	d->Minute = j / 60;
	d->Sec = j % 60 + utow;
	j = 0;
	for (i = 1; j + (k = nav_Months[i] + nav_time_IsLongYear(d->Year, i)) < doy; ++i) j += k;

	d->Month = i;
	d->Day = doy - j;

	nav_time_Date2JD(d);
}

void nav_time_DateToGPSt(nav_Date_t *d, nav_GPSt_t *t)
{
	int dom = 0, doy = 0, m = 0, dow = 0;

	if (d->Year < 1980 ||
		d->Month < 1 ||
		d->Month > 12 ||
		d->Day   > 31) return;

	for (m = 1980; m < d->Year; m++) doy += nav_time_GetDayNumOfYear(m);
	for (m = 1; m < d->Month; m++) dom += nav_time_GetDayOfMonth(d->Year, m);

	dom += d->Day - 6; dow = (dom + doy) % 7;

	t->Week = (dom + doy) / 7;
	t->Tow = dow * 86400 + d->Hour * 3600 + d->Minute * 60 + d->Sec;
}


static char gnsslog_CalSysPrn(int SatNo, int *prn)
{
	char sys = 0;
	*prn = 0;

	if      (SatNo >= GPS_MIN_PRN && SatNo <= GPS_MAX_PRN){
		*prn = SatNo - GPS_MIN_PRN + 1; sys = 'G';
	}
	else if (SatNo >= GLO_MIN_PRN && SatNo <= GLO_MAX_PRN){
		*prn = SatNo - GLO_MIN_PRN + 1; sys = 'R';
	}
	else if (SatNo >= GAL_MIN_PRN && SatNo <= GAL_MAX_PRN){
		*prn = SatNo - GAL_MIN_PRN + 1; sys = 'E';
	}
	else if (SatNo >= BDS_MIN_PRN && SatNo <= BDS_MAX_PRN){
		*prn = SatNo - BDS_MIN_PRN + 1; sys = 'C';
	}

	return sys;
}


static int gnsslog_WriteRnxHeader(gnssRnx_t *gnnsRinex)
{
	fprintf(gnnsRinex->fp,
		"     3.03           OBSERVATION DATA    M: Mixed            RINEX VERSION / TYPE\n");
	fprintf(gnnsRinex->fp, 
		"Log2rnx                                                     PGM / RUN BY / DATE\n");
	fprintf(gnnsRinex->fp,
		"Log2rnx                                                     MARKER NAME\n");
	fprintf(gnnsRinex->fp,
		"RINEX Logger user   Log2rnx                                 OBSERVER / AGENCY\n");
	fprintf(gnnsRinex->fp,
		"                    NONE                NON-NONE            REC # / TYPE / VERS\n");
	fprintf(gnnsRinex->fp,
		"                    NONE                                    ANT # / TYPE\n");
	fprintf(gnnsRinex->fp,
		"        0.0           0.0           0.0                     APPROX POSITION XYZ\n");
	fprintf(gnnsRinex->fp,
		"        0.0000        0.0000        0.0000                  ANTENNA: DELTA H/E/N\n");
	fprintf(gnnsRinex->fp,
		"G   12 C1C L1C D1C S1C C2W L2W D2W S2W C5X L5X D5X S5X      SYS / # / OBS TYPES\n");
	fprintf(gnnsRinex->fp,
		"R    8 C1C L1C D1C S1C C2C L2C D2C S2C                      SYS / # / OBS TYPES\n");
	fprintf(gnnsRinex->fp,
		"E   12 C1C L1C D1C S1C C5X L5X D5X S5X C7X L7X D7X S7X      SYS / # / OBS TYPES\n");
	fprintf(gnnsRinex->fp,
		"C   12 C1I L1I D1I S1I C7I L7I D7I S7X C6I L6I D6I S6I      SYS / # / OBS TYPES\n");
	fprintf(gnnsRinex->fp,
		"  %04d    %02d    %02d    %02d    %02d   %10.7f     GPS         TIME OF FIRST OBS\n",
		gnnsRinex->obsd.t.year, gnnsRinex->obsd.t.month , gnnsRinex->obsd.t.day,
		gnnsRinex->obsd.t.hour, gnnsRinex->obsd.t.minute, gnnsRinex->obsd.t.sec);
	fprintf(gnnsRinex->fp,
		"                                                            END OF HEADER\n");

	return 1;
}

int nav_sat_GetSysPrn(int SatNo, int *Prn)
{
	int i;

	for (i = 0; i < NAV_SYSNUM; ++i) {
		if (SatNo >= g_nav_MinSatNo[i] && SatNo <= g_nav_MaxSatNo[i]) {
			if (Prn != NULL) {
				*Prn = SatNo - g_nav_MinSatNo[i] + 1;
			}
			return i;
		}
	}

	return -1;
}

/*convert string to global satellite prn.*/
int nav_sat_CovSatNo(char *strPrn)
{
	int Prn, SysId, SatNo;
	char Sys;

	if (!strPrn) return 0;

	sscanf(strPrn, "%c%d", &Sys, &Prn);

	if (Sys == ' ') Sys = 'G';

	if ((SysId = nav_sys_GetSysId(Sys)) < 0) return 0;

	SatNo = Prn + g_nav_MinSatNo[SysId] - 1;

	if (SatNo > g_nav_MaxSatNo[SysId]) return 0;

	return SatNo;
}

int nav_sat_CovSatStr(int SatNo, char *strPrn)
{
	int Prn, SysId;
	char SysFlg;

	if (!strPrn || SatNo <= 0) return 0;

	SysId = nav_sat_GetSysPrn(SatNo, &Prn);
	if (SysId < 0) {
		return -1;
	}
	SysFlg = nav_sys_GetSysFlag(SysId);
	if (SysFlg < 0) {
		return -1;
	}
	sprintf(strPrn, "%c%02d", SysFlg, Prn);

	return SysId;
}


static int gnsslog_WriteRnx2(gnssRnx_t *gnnsRinex)
{
	int i = 0, j = 0, state = 0, prn = 0;
	char sys = 0, strsat[10]="";
	nav_ObsEpoch_t *p = &gnnsRinex->ObsData;
	nav_Date_t t = {0};

	if (gnnsRinex->header_flag == 0){
		gnsslog_WriteRnxHeader(gnnsRinex);
		gnnsRinex->header_flag = 1;
	}

	nav_time_GPStToDate(&p->t, &t);

	fprintf(gnnsRinex->fp, "> %04d %02d %02d %02d %02d %10.7f %2d%3d\n",
		t.Year, t.Month , t.Day, t.Hour, t.Minute, t.Sec,
		state, p->SatNum
		);

	for (i = 0; i < p->SatNum; ++i){
		if (nav_sat_CovSatStr(p->SatData[i].SatNo, strsat) < 0){
			continue;
		}
		fprintf(gnnsRinex->fp, "%s", strsat);
		for (j = 0; j < RAW_NFREQ; ++j){
			if (fabs(p->SatData[i].P[j])   > 0.01) fprintf(gnnsRinex->fp, "%14.3f  ", p->SatData[i].P[j]);
			else                             fprintf(gnnsRinex->fp, "                ");
			if (fabs(p->SatData[i].L[j])   > 0.01) fprintf(gnnsRinex->fp, "%14.3f%d ", p->SatData[i].L[j], (int)p->SatData[i].LLI[j]);
			else                             fprintf(gnnsRinex->fp, "                ");
			if (fabs(p->SatData[i].D[j])   > 0.01) fprintf(gnnsRinex->fp, "%14.3f  ", p->SatData[i].D[j]);
			else                             fprintf(gnnsRinex->fp, "                ");
			if (fabs(p->SatData[i].SNR[j]) > 0.01) fprintf(gnnsRinex->fp, "%14.3f  ", (double)p->SatData[i].SNR[j]);
			else                             fprintf(gnnsRinex->fp, "                ");
		}
		fprintf(gnnsRinex->fp, "\n");
	}

	return 1;
}


int nav_gnssconv_json2Obs(int len, nav_U1 *strjs, nav_ObsEpoch_t *o)
{
	cJSON *root = NULL, *time = NULL, *obst = NULL, *tmp = NULL, *item = NULL, *gga;
	cJSON *sats = NULL, *type = NULL;
	int i = 0, j = 0, k = 0, n = 0, m = 0, satcount = 0, sat, prn, LLI, Freq, satno, F;
	double D, P, L, S, vP = 0, vL = 0, vD = 0;
	nav_U1 code[4] = "", sys, strsat[20], *pSec = NULL, vSec[100]="";
	nav_Date_t t = {0};
	nav_ObsSat_t *pSat = NULL;
	int syssats[6] = {0};

	memset(o, 0, sizeof(nav_ObsEpoch_t));

	if (!(root = cJSON_Parse(strjs))){
		LOGI("convert json faild, invalid json string.\n");
		return 0;
	}
	if ((gga = cJSON_GetObjectItem(root, "gga")) == NULL){
		LOGI("convert json faild, no gga word.\n");
		cJSON_Delete(root);
		return 0;
	}
	

	if ((obst = cJSON_GetObjectItem(root, "obsd_t")) == NULL){
		LOGI("convert json faild, no obsd_t word.\n");
		cJSON_Delete(root);
		return 0;
	}
	if ((time = cJSON_GetObjectItem(obst, "t")) == NULL){
		LOGI("convert json faild, no time word.\n");
		cJSON_Delete(root);
		return 0;
	}
	tmp = cJSON_GetObjectItem(time, "year");   if (tmp != NULL) t.Year   = tmp->valueint;
	tmp = cJSON_GetObjectItem(time, "month");  if (tmp != NULL) t.Month  = tmp->valueint;
	tmp = cJSON_GetObjectItem(time, "day");    if (tmp != NULL) t.Day    = tmp->valueint;
	tmp = cJSON_GetObjectItem(time, "hour");   if (tmp != NULL) t.Hour   = tmp->valueint;
	tmp = cJSON_GetObjectItem(time, "minute"); if (tmp != NULL) t.Minute = tmp->valueint;
	tmp = cJSON_GetObjectItem(time, "sec");    if (tmp != NULL) t.Sec    = tmp->valuedouble;

	if (t.Year <= 1980){
		LOGI("convert json faild, invalid time, year=%d.\n", t.Year);
		cJSON_Delete(root);
		return 0;
	}


	tmp = cJSON_GetObjectItem(obst, "satnum"); if (tmp != NULL) satcount = tmp->valueint;
	if (satcount <= 0){
		LOGI("convert json faild, invalid satnum, satcount=%d.\n", satcount);
		cJSON_Delete(root);
		return 0;
	}
	if ((sats = cJSON_GetObjectItem(obst, "d")) == NULL){
		LOGI("convert json faild, no sats data.\n");
		cJSON_Delete(root);
		return 0;
	}
	if ((n = cJSON_GetArraySize(sats)) != satcount){
		LOGI("convert json faild, satcount=%d != ArraySize=%d\n", satcount, n);
		cJSON_Delete(root);
		return 0;
	}

	for (i = n = 0; i < satcount; ++i){
		if ((item = cJSON_GetArrayItem(sats, i)) == NULL){
			break;
		}
		D = L = P = S = vD = vL = vP = 0.0; LLI = 0; prn = 0;
		code[0] = code[1] = code[2] = '\0';
		tmp = cJSON_GetObjectItem(item,    "d"); if (tmp != NULL) D   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,    "l"); if (tmp != NULL) L   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,    "p"); if (tmp != NULL) P   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,   "vd"); if (tmp != NULL)vD   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,   "vl"); if (tmp != NULL)vL   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,   "vp"); if (tmp != NULL)vP   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item,  "lli"); if (tmp != NULL) LLI = tmp->valueint;
		tmp = cJSON_GetObjectItem(item,  "snr"); if (tmp != NULL) S   = tmp->valuedouble;
		tmp = cJSON_GetObjectItem(item, "type"); if (tmp != NULL) strcpy(code, tmp->valuestring);
		tmp = cJSON_GetObjectItem(item,  "prn"); if (tmp != NULL) sat = tmp->valueint;
		Freq = -1;
		sys = gnsslog_CalSysPrn(sat, &prn);
		sprintf(strsat, "%c%02d", sys, prn);
		satno = nav_sat_CovSatNo(strsat);
		if (sys == 'C' && code[1] == '1'){
			if (code[2] == 'I'){
				code[1] = '2';
				Freq = 1;
			}
		}
		else Freq = code[1] - '0';
		if (satno <= 0 || Freq <= 0){
			continue;
		}
		if (fabs(vD) > 5.0){
			continue;
		}
		if (P < 0.0){
			continue;
		}
		F = -1;
		//------------------------------------
		if (sys == 'G'){
			if      (Freq == 1) F = 0;
			else if (Freq == 2) F = 1;
			else if (Freq == 5) F = 2;
		}
		//------------------------------------
		else if (sys == 'R'){
			if      (Freq == 1) F = 0;
			else if (Freq == 2) F = 1;
		}
		//------------------------------------
		else if (sys == 'C'){
			if      (Freq == 1) F = 0;
			else if (Freq == 2) F = 0;
			else if (Freq == 7) F = 1;
			else if (Freq == 6) F = 2;
		}
		//------------------------------------
		else if (sys == 'E'){
			if      (Freq == 1) F = 0;
			else if (Freq == 5) F = 1;
			else if (Freq == 8) F = 2;
		}
		if (F < 0){
			continue;
		}
		pSat = NULL;
		for (j = 0; j < o->SatNum; ++j){
			if (o->SatData[j].SatNo == satno){ pSat = &o->SatData[j]; break; }
		}
		if (pSat == NULL && o->SatNum < NAV_MAXSAT){
			pSat = &o->SatData[o->SatNum++];
			pSat->SatNo = satno;
			if      (sys == 'G') syssats[0] += 1;
			else if (sys == 'R') syssats[1] += 1;
			else if (sys == 'E') syssats[2] += 1;
			else if (sys == 'C') syssats[3] += 1;
		}
		//------------------------------------
		if (F >= 0 && pSat != NULL){
			if (fabs(pSat->L[F]) < 0.001 && fabs(L) > 0.001){
				pSat->L[F] = L;
				pSat->LLI[F] = NAV_MAX(pSat->LLI[F], LLI);
			}
			if (fabs(pSat->P [F])  < 0.001 && fabs (P) > 0.001) pSat->P [F]  = P;
			if (fabs(pSat->D [F])  < 0.001 && fabs (D) > 0.001) pSat->D [F]  = (nav_F4)D;
			if (fabs(pSat->vL[F])  < 0.001 && fabs(vL) > 0.001) pSat->vL[F]  = vL*vL;
			if (fabs(pSat->vP[F])  < 0.001 && fabs(vP) > 0.001) pSat->vP[F]  = vP*vP;
			if (fabs(pSat->vD[F])  < 0.001 && fabs(vD) > 0.001) pSat->vD[F]  = vD*vD;
			if (fabs(pSat->SNR[F]) < 0.001 && fabs (S) > 0.001) pSat->SNR[F] = (nav_U1)(S+0.5);
			//pSat->LLI[F] = NAV_MAX(pSat->LLI[F], LLI);
		}
	}
	cJSON_Delete(root);

	nav_time_DateToGPSt(&t, &o->t);

	if (pSec = strstr(strjs, "\"sec\":")){
		pSec += strlen("\"sec\":");
		sscanf(pSec, "%[^,|}|\"]", &vSec);
	}
	LOGI("convert json ok, t=%04d/%02d/%02d %02d:%02d:%10.7f, satn=%d(G=%2d,R=%2d,E=%2d,C=%2d)\n",
		t.Year, t.Month, t.Day, t.Hour, t.Minute, t.Sec,
		o->SatNum, syssats[0], syssats[1], syssats[2], syssats[3]);

	

	return o->SatNum;
}

static int nav_rtk_GnssSpare_Read(rnx_ObsData_t *r)
{
	int len = 0, satn = 0;
	unsigned char buff[8096] = "", *p;

	if (r == NULL ){
		return 0;
	}

	if (feof(r->fp)) return 0;

	while (!feof(r->fp)){
		memset(buff, 0, 8096);
		fgets(buff, 8095, r->fp);
		if ((len = strlen(buff)) < 10){
			continue;
		}
		p = strstr(buff, "{");
		if (p == NULL){
			continue;
		}
		if ((satn = nav_gnssconv_json2Obs(len, buff, &r->ObsEpoch)) > 0){
			break;
		}
	}

	

	return 1;
}

// json rtcm
int nav_rtk_GnssSapreProcess(char *jsonlog, char* output)
{
	int  j=0,ret = 0;
	
	rnx_ObsData_t rnx_Rov = {0};
	char *p = NULL, rnxfile[1024]="";

	gnssRnx_t gnssRnx = {0};

	rnx_Rov.fp = fopen(jsonlog, "r");
	if (rnx_Rov.fp == NULL){
		printf("gnsslog file open error: %s\n", jsonlog);
		return 0;
	}

	if (output){
		gnssRnx.fp = fopen(output, "w");
	}

	while ((ret = nav_rtk_GnssSpare_Read(&rnx_Rov)) > 0){

		// process...
		gnssRnx.ObsData = rnx_Rov.ObsEpoch;
		gnsslog_WriteRnx2(&gnssRnx);
	}

	fclose(rnx_Rov.fp);



	return 1;
}




