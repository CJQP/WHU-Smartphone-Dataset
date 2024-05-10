#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "nav.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include <io.h>
#else
#endif

//#define MY_LOGPATH

#define NAV_MAX_BUFF_LEN   4096
#define NAV_MAX_PATH_LEN   512

#define NAV_DEF2STR(a)    (#a)

static int  g_log_logfl = 1;
static int  g_log_level = 0;
static int  g_log_levsn = NAV_LOG_LEVELN;
static int  g_log_rankf = 0;
static int  g_log_sinfo = 0;
static int  g_log_ttags = 1;
static int  g_log_spans = 7;

#ifdef MY_LOGPATH
static char g_los_path[100] = "../log";
#else
static char g_los_path[100] = "./logs";
#endif

char         *g_log_ranks[NAV_LOG_LEVELN] = {
		NAV_DEF2STR(NAV_LOG_OFF  )+4,
		NAV_DEF2STR(NAV_LOG_ERROR)+4,
		NAV_DEF2STR(NAV_LOG_WARN )+4,
		NAV_DEF2STR(NAV_LOG_TRACE)+4,
		NAV_DEF2STR(NAV_LOG_INFO )+4,
	    NAV_DEF2STR(NAV_LOG_DEBUG)+4
};

typedef struct nav_ProcLog_t{
	nav_U1 file[NAV_MAX_PATH_LEN];
	FILE *fp;
	nav_I4 init;
}nav_ProcLog_t;

nav_ProcLog_t nav_ProcLog = {0};


 
nav_GPSt_t nav_time_GPStAdd(nav_GPSt_t t, nav_F8 dt)
{
	nav_GPSt_t c = t;

	c.Tow += dt;

	if (c.Tow >= 604800.0) { c.Tow -= 604800.0; c.Week += 1; }
	else if (c.Tow < 0.0) { c.Tow += 604800.0; c.Week -= 1; }

	return c;
}


nav_I4 nav_log_TimeCovToBJ(nav_Date_t *t)
{
	nav_GPSt_t tGps = {0};
	
	nav_time_DateToGPSt(t, &tGps);

	tGps = nav_time_GPStAdd(tGps, 8.0*3600);

	nav_time_GPStToDate(&tGps, t);
	
	return 1;
}

int nav_time_LocalTime(double ep[6])
{
	struct timeval tv = { 0 };
	struct tm *tt = NULL;

	memset(ep, 0, 6 * sizeof(double));
#ifdef WIN32
	SYSTEMTIME ts;

	GetSystemTime(&ts); /* utc */
	ep[0] = ts.wYear;
	ep[1] = ts.wMonth;
	ep[2] = ts.wDay;
	ep[3] = ts.wHour;
	ep[4] = ts.wMinute;
	ep[5] = ts.wSecond + ts.wMilliseconds*1E-3;
	return 1;
#else
	if (!gettimeofday(&tv, NULL) && (tt = gmtime(&tv.tv_sec))) {
		ep[0] = tt->tm_year + 1900;
		ep[1] = tt->tm_mon + 1;
		ep[2] = tt->tm_mday;
		ep[3] = tt->tm_hour;
		ep[4] = tt->tm_min;
		ep[5] = tt->tm_sec + tv.tv_usec*1E-6;

		return 1;
	}
#endif
	return 0;
}

void nav_time_GetLocalDate(nav_Date_t *t)
{
	nav_F8 ep[6] = { 0 };

	nav_time_LocalTime(ep);
	t->Year = (nav_I4)ep[0]; t->Month = (nav_I4)ep[1]; t->Day = (nav_I4)ep[2];
	t->Hour = (nav_I4)ep[3]; t->Minute = (nav_I4)ep[4]; t->Sec = ep[5];

	nav_time_Date2JD(t);
}

nav_I4 nav_log_PrintLog(nav_I4 level, nav_I1 *fmt, ...)
{
	nav_I1 buffer[NAV_MAX_BUFF_LEN] = "", outfile[300] = "";
	nav_I4 n = 0, timetag = 0, CovToBJ = 1;
	nav_Date_t t = {0};

	if(level == 0 || level > g_log_level || level >= g_log_levsn) return 0;

	if(g_log_ttags){
		nav_time_GetLocalDate(&t);
		if(CovToBJ > 0) nav_log_TimeCovToBJ(&t);
		timetag = 1;
		// time tag.
		n += sprintf(buffer + n, "[%04d/%02d/%02d %02d:%02d:%02d] ",
			t.Year, t.Month, t.Day, t.Hour, t.Minute, (int)(t.Sec));
	}

	if(g_log_rankf){
		// log lovel.
		n += sprintf(buffer + n, "[%s] ", g_log_ranks[level]);
	}

	if (g_log_logfl){
		if (timetag == 0){
			nav_time_GetLocalDate(&t); timetag = 1;
			if(CovToBJ > 0) nav_log_TimeCovToBJ(&t);
		}
		if (timetag){
			sprintf(outfile, "%s/%s-%04d-%02d-%02d.log", g_los_path, "rtknav",
				t.Year, t.Month, t.Day);
			if (strcmp(outfile, nav_ProcLog.file) || nav_ProcLog.fp == NULL){
				if (nav_ProcLog.fp) fclose(nav_ProcLog.fp);
				nav_ProcLog.fp = fopen(outfile, "a+");
				if (nav_ProcLog.fp){
					strcpy(nav_ProcLog.file, outfile);
					setvbuf(nav_ProcLog.fp, NULL, _IONBF, 0);
				}
			}
		}
	}

	va_list v;
	va_start(v, fmt);
	vsnprintf(buffer + n, sizeof(buffer), fmt, v);

	
	//printf("%s", buffer); //fflush(stdout);
	if (g_log_logfl && nav_ProcLog.fp){
		if (fprintf(nav_ProcLog.fp, "%s", buffer) < 0){
			fclose(nav_ProcLog.fp); nav_ProcLog.fp = NULL;
		}
		else{
			//fflush(g_proclog.fp);
		}
	}

	va_end(v);

	return 1;
}

nav_I4 nav_rtklib_log_PrintLog(nav_I1 *fmt, ...)
{
	nav_I1 buffer[NAV_MAX_BUFF_LEN] = "", outfile[300] = "";
	nav_I4 n = 0, timetag = 0, CovToBJ = 1;
	nav_I1 level = NAV_LOG_INFO;
	nav_Date_t t = {0};

	if(level == 0 || level >= g_log_levsn) return 0;

	//if(g_log_ttags){
		nav_time_GetLocalDate(&t);
		if (CovToBJ > 0) nav_log_TimeCovToBJ(&t);
		timetag = 1;
		// time tag.
		n += sprintf(buffer + n, "[%04d/%02d/%02d %02d:%02d:%02d] ",
			t.Year, t.Month, t.Day, t.Hour, t.Minute, (int)(t.Sec));
	//}

	if(g_log_rankf){
		// log lovel.
		n += sprintf(buffer + n, "[%s] ", g_log_ranks[level]);
	}

	if (g_log_logfl){
		if (timetag == 0){
			nav_time_GetLocalDate(&t); timetag = 1;
			if (CovToBJ > 0) nav_log_TimeCovToBJ(&t);
		}
		if (timetag){
			sprintf(outfile, "%s/%s-%04d-%02d-%02d.log", g_los_path, "rtknav",
				t.Year, t.Month, t.Day);
			if (strcmp(outfile, nav_ProcLog.file) || nav_ProcLog.fp == NULL){
				if (nav_ProcLog.fp) fclose(nav_ProcLog.fp);
				nav_ProcLog.fp = fopen(outfile, "a+");
				if (nav_ProcLog.fp){
					strcpy(nav_ProcLog.file, outfile);
					setvbuf(nav_ProcLog.fp, NULL, _IONBF, 0);
				}
				else{
					printf("failed to open logfile:%s\n", outfile);
				}
			}
		}
	}

	va_list v;
	va_start(v, fmt);
	vsnprintf(buffer + n, sizeof(buffer), fmt, v);

	
	printf("%s", buffer); //fflush(stdout);
	if (g_log_logfl && nav_ProcLog.fp){
		if (fprintf(nav_ProcLog.fp, "%s", buffer) < 0){
			fclose(nav_ProcLog.fp); nav_ProcLog.fp = NULL;
		}
		else{
			//fflush(g_proclog.fp);
		}
	}

	va_end(v);

	return 1;
}
