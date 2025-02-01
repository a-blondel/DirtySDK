/*H********************************************************************************/
/*!
    \File platform.h

    \Description
        A platform wide header that performs environment identification and replaces
        some standard lib functions with "safe" alternatives (such an snprintf that
        always terminates the buffer).

    \Copyright
        Copyright (c) 2005 Electronic Arts Inc.

    \Version 01/25/2005 (gschaefer) First Version
*/
/********************************************************************************H*/

#ifndef _platform_h
#define _platform_h

/*** Include files ****************************************************************/
#if defined(__APPLE__) && __APPLE__
    #include <TargetConditionals.h>  // for TARGET_OS_IPHONE and TARGET_IPHONE_SIMULATOR
#endif

/*** Defines **********************************************************************/

// define the different platforms
#ifndef DIRTYCODE_UNDEF
#define DIRTYCODE_UNDEF         1   //!< Compiled on undefined platform
#define DIRTYCODE_PC            3   //!< Compiled for Windows
#define DIRTYCODE_APPLEIOS      4   //!< Compiled for Apple IOS (iPhone, iPad, etc)
#define DIRTYCODE_ANDROID       5   //!< Compiled for Android
#define DIRTYCODE_PLAYBOOK      6   //!< Compiled for Playbook
#define DIRTYCODE_XENON         10  //!< Compiled for Microsoft Xenon
#define DIRTYCODE_LINUX         11  //!< Compiled for Linux
#define DIRTYCODE_PS3           12  //!< Compiled for Sony PS3 PPU
#define DIRTYCODE_REVOLUTION    14  //!< Compiled for Nintendo Revolution
#endif

// identify platform if not yet known
#ifdef DIRTYCODE_PLATFORM
// nothing
#elif defined (_XBOX)
#define DIRTYCODE_PLATFORM DIRTYCODE_XENON
#elif defined (_WIN32) || defined (_WIN64)
#define DIRTYCODE_PLATFORM DIRTYCODE_PC
#elif defined (__CELLOS_LV2__)
#define DIRTYCODE_PLATFORM DIRTYCODE_PS3
#elif defined(__MWERKS__) && defined(RVL_SDK)
#define DIRTYCODE_PLATFORM DIRTYCODE_REVOLUTION
#elif defined(__IPHONE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#define DIRTYCODE_PLATFORM DIRTYCODE_APPLEIOS
#elif defined(__ANDROID__) || defined (ANDROID)
#define DIRTYCODE_PLATFORM DIRTYCODE_ANDROID
#elif defined(__QNX__) || defined (__QNXNTO__)
#define DIRTYCODE_PLATFORM DIRTYCODE_PLAYBOOK
#elif defined(__linux__)
#define DIRTYCODE_PLATFORM DIRTYCODE_LINUX
#else
#error DIRTYCODE_PLATFORM was not predefined and could not be auto-determined!
#endif

// define platform name
#if DIRTYCODE_PLATFORM == DIRTYCODE_ANDROID
#define DIRTYCODE_PLATNAME "Android"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_APPLEIOS
#define DIRTYCODE_PLATNAME "AppleIOS"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_LINUX
#define DIRTYCODE_PLATNAME "Linux"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_PC
#define DIRTYCODE_PLATNAME "Windows"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_PLAYBOOK
#define DIRTYCODE_PLATNAME "Playbook"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_PS3
#define DIRTYCODE_PLATNAME "PlayStation3"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_REVOLUTION
#define DIRTYCODE_PLATNAME "Wii"
#elif DIRTYCODE_PLATFORM == DIRTYCODE_XENON
#define DIRTYCODE_PLATNAME "Xbox360"
#else
#define DIRTYCODE_PLATNAME "Undefined"
#endif

// define 32-bit or 64-bit pointers
#if defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__mips64__) || defined(__64BIT__) 
 #define DIRTYCODE_64BITPTR (1)
#elif defined(__CC_ARM) && (__sizeof_ptr == 8)
 #define DIRTYCODE_64BITPTR (1)
#else
 #define DIRTYCODE_64BITPTR (0)
#endif

// we need va_list to be a universal type
#include <stdarg.h>

// c99 types for all platforms - check for defines for eabase compatibility
#if ((DIRTYCODE_PLATFORM == DIRTYCODE_PC) || \
    (DIRTYCODE_PLATFORM == DIRTYCODE_XENON))
    #ifndef __int8_t_defined
        typedef signed char             int8_t;
        typedef signed short            int16_t;
        typedef signed int              int32_t;
        typedef signed long long        int64_t;
        #define __int8_t_defined
    #endif
    #ifndef __uint32_t_defined
        typedef unsigned char           uint8_t;
        typedef unsigned short          uint16_t;
        typedef unsigned int            uint32_t;
        typedef unsigned long long      uint64_t;
        #define __uint32_t_defined
    #endif
    #ifndef _intptr_t_defined
        #if defined(_WIN64)
            typedef signed long long    intptr_t;
        #else
            typedef signed int          intptr_t;
        #endif
        #define _intptr_t_defined
    #endif
    #ifndef _uintptr_t_defined
        #if defined(_WIN64)
            typedef unsigned long long  uintptr_t;
        #else
            typedef unsigned int        uintptr_t;
        #endif
        #define _uintptr_t_defined
    #endif
#else
    // other platforms supply stdint, so just include that
    #ifndef __uint32_t_defined
        #include <stdint.h>
    #endif
#endif

#include <time.h>

/*** Defines **********************************************************************/

// force our definition of TRUE/FALSE
#ifdef  TRUE
#undef  TRUE
#undef  FALSE
#endif

#define FALSE (0)
#define TRUE  (1)

// force our definitions of NULL
#ifdef  NULL
#undef  NULL
#endif

#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif

// map common debug code definitions to our debug code definition
#ifndef DIRTYCODE_DEBUG
 #if defined(EA_DEBUG)
  #define DIRTYCODE_DEBUG (1)
 #elif defined(RWDEBUG)
  #define DIRTYCODE_DEBUG (1)
 #else
  #define DIRTYCODE_DEBUG (0)
 #endif
#endif

/*** Macros ***********************************************************************/

/*! macros to redefine common function names to their dirtysock functional
    equivalents.  these macros do not compile in by default; if they are
    desired, DS_PLATFORM must be defined in the project/makefile. */
#ifdef DS_PLATFORM

//! platform debug print function
#if DIRTYCODE_LOGGING
 #define logprintf(_x) _Platform_pLogPrintf _x
#else
 #define logprintf(_x) { }
#endif

// make sure that replacement functions use our versions
#define vsnzprintf ds_vsnzprintf
#define snzprintf ds_snzprintf
#define strnzcpy ds_strnzcpy
#define strcasecmp ds_stricmp
#define strncasecmp ds_strnicmp
#ifndef stricmp
#define stricmp ds_stricmp
#endif
#ifndef strnicmp
#define strnicmp ds_strnicmp
#endif
#define stristr ds_stristr
#define localtime ds_localtime

// custom functions, named similarly to standard string functions
#define strsubzcpy ds_strsubzcpy
#define strnzcat ds_strnzcat
#define strsubzcat ds_strsubzcat

// these are special time related functions -- new names avoid issues
#define timezone ds_timezone
#define timetosecs ds_timetosecs
#define secstotime ds_secstotime
#define timeinsecs ds_timeinsecs
#define plattimetotime ds_plattimetotime
#define timetostr ds_timetostr
#define strtotime ds_strtotime
#endif // DS_PLATFORM

/*** Type Definitions *************************************************************/

//! time-to-string conversion type
typedef enum _TimeToSringConversionTypeE
{
    TIMETOSTRING_CONVERSION_ISO_8601  //!< ISO 8601 standard:  yyyy-MM-ddTHH:mmZ where Z means 0 UTC offset, and no Z means local time zone
} TimeToSringConversionTypeE;

/*** Variables ********************************************************************/

//! this variable is not implemented in platform, it must be implemented in client code
#if DIRTYCODE_LOGGING
extern int (*_Platform_pLogPrintf)(const char *pFmt, ...);
#endif

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// replacement time functions
uint32_t ds_timeinsecs(void);
int32_t ds_timezone(void);
struct tm *ds_localtime(struct tm *tm, uint32_t elap);
struct tm *ds_secstotime(struct tm *tm, uint32_t elap);
uint32_t ds_timetosecs(const struct tm *tm);
struct tm *ds_plattimetotime(struct tm *tm, void *pPlatTime);
char * ds_timetostr(const struct tm *tm, TimeToSringConversionTypeE eConvType, uint8_t bLocalTime, char *pStr);
uint32_t ds_strtotime(const char *str);

// replacement string functions
int32_t ds_vsnprintf(char *pBuffer, int32_t iLength, const char *pFormat, va_list Args);
int32_t ds_vsnzprintf(char *pBuffer, int32_t iLength, const char *pFormat, va_list Args);
int32_t ds_vsnzprintf2(char *pBuffer, int32_t iLength, const char *pFormat, va_list Args);
int32_t ds_snzprintf(char *pBuffer, int32_t iLength, const char *pFormat, ...);
int32_t ds_snzprintf2(char *pBuffer, int32_t iLength, const char *pFormat, ...);
char *ds_strnzcpy(char *pDest, const char *pSource, int32_t iCount);
int32_t ds_strnzcat(char *pDst, const char *pSrc, int32_t iDstLen);
int32_t ds_stricmp(const char *pString1, const char *pString2);
int32_t ds_strnicmp(const char *pString1, const char *pString2, uint32_t uCount);
char *ds_stristr(const char *pHaystack, const char *pNeedle);

// 'original' string functions
int32_t ds_strsubzcpy(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen);
int32_t ds_strsubzcat(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen);

#ifdef __cplusplus
}
#endif

#endif // _platform_h
