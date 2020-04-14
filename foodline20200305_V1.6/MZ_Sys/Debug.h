#ifndef MZSYS_DEBUG_HEADER_H
#define MZSYS_DEBUG_HEADER_H



#define SOFT_BUILD_VERSION  0x2018010E      // year, main version, vice version
#define SOFT_BUILD_TIME     0x10201733      // month, date and time hex->decimal


#define ENABLE_JTAG_DEBUG       0
#define USE_RTC_WITH_BATTERY    1


// debug switch
#define OS_WRAP_DBG     0
#define OS_HEAP_DBG     0
#define AIRIAP_DBG      0
#define WIRELESS_DBG    0
#define WLAN4G_DBG      0
#define DISPATCHER_DBG  0

#define MZ_PRINT_DEBUG  (OS_WRAP_DBG|OS_HEAP_DBG|AIRIAP_DBG|WIRELESS_DBG|WLAN4G_DBG)



#if MZ_PRINT_DEBUG

#include <stdio.h>
#define Debug printf

#else

#define Debug(x,y...)

#endif

#endif

