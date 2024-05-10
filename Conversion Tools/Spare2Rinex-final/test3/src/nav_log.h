#ifndef NAV_LOG_HH_H
#define NAV_LOG_HH_H

#define RTK_LOG

#include <stdio.h>
#include "nav.h"

nav_I4 nav_rtklib_log_PrintLog(nav_I1 *fmt, ...);

#ifndef _WIN32
/*#include <android/log.h>

#define LOG_TAG "RtkLib"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%s:%d]"fmt,  __FUNCTION__, __LINE__,##args)
#define LOGW(fmt, args...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%s:%d]"fmt,__FUNCTION__, __LINE__, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%s:%d]"fmt,__FUNCTION__, __LINE__, ##args)
*/
#ifdef RTK_LOG
#define LOGI nav_rtklib_log_PrintLog
#define LOGW nav_rtklib_log_PrintLog
#define LOGE nav_rtklib_log_PrintLog
#else
#define LOG_TAG "RtkLib"
#define LOGI printf
#define LOGW printf
#define LOGE printf
#endif
#else
#ifdef RTK_LOG
#define LOGI nav_rtklib_log_PrintLog
#define LOGW nav_rtklib_log_PrintLog
#define LOGE nav_rtklib_log_PrintLog
#else
#define LOG_TAG "RtkLib"
#define LOGI printf
#define LOGW printf
#define LOGE printf
#endif
#endif

#endif