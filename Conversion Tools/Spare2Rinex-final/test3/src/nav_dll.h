#ifndef RTKNAV_DLL_HH_H
#define RTKNAV_DLL_HH_H

#include "nav.h"

#ifdef _WIN32
//#define STR_TEST
#define TEST_EXPORT
#else
#define TEST_EXPORT
#endif

#ifdef TEST_EXPORT
#ifdef _WIN32
#define RTKEXPORT __declspec(dllexport)
#else
#define RTKEXPORT __attribute__ ((visibility("default")))
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

 
//spare
RTKEXPORT int nav_rtk_GnssSapreProcess(char *jsonlog, char* output);


#ifdef __cplusplus
}
#endif

#endif