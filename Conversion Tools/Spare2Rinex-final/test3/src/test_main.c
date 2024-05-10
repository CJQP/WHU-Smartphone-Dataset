#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <signal.h>
#include "nav.h"
#include "nav_dll.h"


int main()
{

	nav_rtk_GnssSapreProcess(
		"../data/2020-08-07/2020_08_07_20_03_45.604_spare_log_XIM8.txt",
		"../data/2020-08-07/2020_08_07_20_03_45.604_spare_log_XIM8-1.obs"
	);

	return 1;
}